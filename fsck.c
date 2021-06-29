#include "types.h"
#include "fs.h"
#include "param.h"
#include "x86.h"
#include "defs.h"
#include "mmu.h"
#include "stat.h"

#define DIRPB (BSIZE / sizeof(struct dirent))
#define NCACHE (PGSIZE / BSIZE)
#define INODE_COUNT 200
#define BLOCK_COUNT 1000

static void readblock(void* buf, uint block)
{
  while((inb(0x1F7) & 0xC0) != 0x40);
  outb(0x1F2, 1);
  outb(0x1F3, block);
  outb(0x1F4, block >> 8);
  outb(0x1F5, block >> 16);
  outb(0x1F6, (block >> 24) | 0xE0 | ((ROOTDEV & 1) << 4));
  outb(0x1F7, 0x20);

  while((inb(0x1F7) & 0xC0) != 0x40);
  insl(0x1F0, buf, BSIZE / 4);
}

static void writeblock(void* buf, uint block)
{
  while((inb(0x1F7) & 0xC0) != 0x40);
  outb(0x1F2, 1);
  outb(0x1F3, block);
  outb(0x1F4, block >> 8);
  outb(0x1F5, block >> 16);
  outb(0x1F6, (block >> 24) | 0xE0 | ((ROOTDEV & 1) << 4));
  outb(0x1F7, 0x30);

  while((inb(0x1F7) & 0xC0) != 0x40);
  outsl(0x1F0, buf, BSIZE / 4);
}

struct lrucache {
  struct node {
    uint block;
    char dirty;
    void *data;
    struct node *prev, *next;
  } nodes[NCACHE];
  uint count;
  struct node *head, *tail;
  void *mem;
};

static void lruinit(struct lrucache *cache)
{
  cache->count = 0;
  cache->head = cache->tail = 0;
  cache->mem = (void *)kalloc();
  for (int i = 0; i < NCACHE; ++i)
    cache->nodes[i].data = cache->mem + BSIZE * i;
}

static void lrufree(struct lrucache *cache)
{
  for (struct node *p = cache->head; p; p = p->next)
    if (p->dirty)
      writeblock(p->data, p->block);
  kfree((char *)cache->mem);
}

static void *lruget(struct lrucache *cache, uint block)
{
  for (struct node *p = cache->head; p; p = p->next)
    if (p->block == block) {
      // Found
      if (p == cache->head)
        return p->data;
      // Bring p to head
      p->prev->next = p->next;
      if (p != cache->tail)
        p->next->prev = p->prev;
      else
        cache->tail = p->prev;
      p->prev = 0;
      p->next = cache->head;
      cache->head->prev = p;
      cache->head = p;
      return p->data;
    }
  // Not found
  struct node *newnode;
  if (cache->count < NCACHE) {
    newnode = &cache->nodes[cache->count];
    ++cache->count;
  } else {
    if (cache->tail->dirty)
      writeblock(cache->tail->data, cache->tail->block);
    newnode = cache->tail;
    cache->tail = newnode->prev;
    cache->tail->next = 0;
  }
  // Setup node
  readblock(newnode->data, block);
  newnode->block = block;
  newnode->dirty = 0;
  if (cache->head) {
    newnode->next = cache->head;
    newnode->prev = 0;
    cache->head->prev = newnode;
    cache->head = newnode;
  } else {
    newnode->next = newnode->prev = 0;
    cache->head = cache->tail = newnode;
  }
  return newnode->data;
}

static struct dinode *getinode(struct lrucache *cache, uint inode)
{
  uint block = 32 + inode / IPB;
  struct dinode *data = (struct dinode *)lruget(cache, block);
  return &data[inode % IPB];
}

static struct dirent *getdirents(struct lrucache *cache, struct dinode *inode)
{
  if (inode->type != T_DIR)
    panic("getdirents");
  uint block = inode->addrs[0];
  return (struct dirent *)lruget(cache, block);
}

static uint *getindirects(struct lrucache *cache, struct dinode *inode)
{
  uint block = inode->addrs[NDIRECT];
  return (uint *)lruget(cache, block);
}

static void lrudirty(struct lrucache *cache)
{
  cache->head->dirty = 1;
}

static void fsckrec(struct lrucache *cache, uchar *visited, uint inode)
{
  visited[inode / 8] |= 1 << (inode % 8);
  struct dinode *ip = getinode(cache, inode);
  if (ip->type != T_DIR)
    return;
  struct dirent *dp = getdirents(cache, ip);
  for (int i = 2; i < DIRPB; ++i)
    if (dp[i].inum != 0)
      fsckrec(cache, visited, dp[i].inum);
}

static int freeblock(struct lrucache *cache, uint block)
{
  uint b = block / BPB + 58;
  uint off = block % BPB / 8;
  uint mask = 1 << (block % BPB % 8);

  uchar *bitmap = (uchar *)lruget(cache, b);
  if (!(bitmap[off] & mask))
    return 0;
  bitmap[off] &= ~mask;
  return 1;
}

static void fixblock(struct lrucache *cache, struct dinode *inode, uint *blockcount)
{
  uint addr;
  for (int j = 0; j < NDIRECT; ++j)
    if ((addr = inode->addrs[j]))
      *blockcount += freeblock(cache, addr);
  if ((addr = inode->addrs[NDIRECT])) {
    *blockcount += freeblock(cache, addr);
    uint *idp = getindirects(cache, inode);
    for (int j = 0; j < NINDIRECT; ++j)
      if ((addr = idp[j]))
        *blockcount += freeblock(cache, addr);
  }
}

static void fixinodes(struct lrucache *cache, uchar *visited, uint *inodecount, uint *blockcount)
{
  *inodecount = *blockcount = 0;
  for (int i = 0; i < INODE_COUNT; ++i) {
    struct dinode *ip = getinode(cache, i);
    int v = visited[i / 8] & (1 << (i % 8));
    if (!v && ip->type != 0) {
      cprintf("fsck: inode %d is allocated but is not referenced by any dir! Fixing... ", i);
      ip->type = 0;
      lrudirty(cache);
      ++*inodecount;
      fixblock(cache, ip, blockcount);
      cprintf("Done!\n");
    }
  }
}

void fsck()
{
  cprintf("Running fsck...\n");
  // Init
  struct lrucache cache;
  lruinit(&cache);
  // Check
  uchar visited[INODE_COUNT / 8] = { 0 };
  fsckrec(&cache, visited, 1);
  // Fix
  uint inodecount = 0, blockcount = 0;
  fixinodes(&cache, visited, &inodecount, &blockcount);
  // Done
  lrufree(&cache);
  cprintf("fsck completed: ");
  if (inodecount == 0 && blockcount == 0)
    cprintf("no problem found\n");
  else
    cprintf("Fixed %d inodes and freed %d disk blocks.\n", inodecount, blockcount);
}
