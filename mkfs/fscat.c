#include "kernel/fs.h"
#include "kernel/stat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint ktoh(uint v)
{
  uchar* p = (uchar*)&v;
  return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

static uint16 ktohs(uint16 v)
{
  uchar* p = (uchar*)&v;
  return p[0] | (p[1] << 8);
}

static void read_block(FILE* fs, size_t block, void* buf)
{
  if (fseek(fs, block * BSIZE, SEEK_SET) == -1) {
    perror("fseek");
    exit(EXIT_FAILURE);
  }

  if (fread(buf, BSIZE, 1, fs) != 1) {
    perror("fread");
    exit(EXIT_FAILURE);
  }
}

static void read_sb(FILE* fs, struct superblock* sb)
{
  char buf[BSIZE];
  read_block(fs, 1, buf);

  memcpy(sb, buf, sizeof(*sb));
  sb->magic = ktoh(sb->magic);
  sb->size = ktoh(sb->size);
  sb->nblocks = ktoh(sb->nblocks);
  sb->ninodes = ktoh(sb->ninodes);
  sb->nlog = ktoh(sb->nlog);
  sb->logstart = ktoh(sb->logstart);
  sb->inodestart = ktoh(sb->inodestart);
  sb->bmapstart = ktoh(sb->bmapstart);
}

static void read_dinode(FILE* fs, const struct superblock* sb,
                        size_t inode, struct dinode* dinode)
{
  size_t block = IBLOCK(inode, *sb);

  char buf[BSIZE];
  read_block(fs, block, buf);

  struct dinode* buf_dinode = (struct dinode*)buf + (inode % IPB);
  dinode->type = ktohs(buf_dinode->type);
  dinode->major = ktohs(buf_dinode->major);
  dinode->minor = ktohs(buf_dinode->minor);
  dinode->nlink = ktohs(buf_dinode->nlink);
  dinode->size = ktoh(buf_dinode->size);

  for (size_t i = 0; i < NDIRECT; ++i) {
    dinode->addrs[i] = ktoh(buf_dinode->addrs[i]);
  }
}

static void read_inode_block(FILE* fs, const struct dinode* dinode,
                             size_t block, void* buf)
{
  if (block > NDIRECT) {
    fprintf(stderr, "Indirect blocks not supported\n");
    exit(EXIT_FAILURE);
  }

  read_block(fs, dinode->addrs[block], buf);
}

static void read_inode_dirent_block(FILE* fs, const struct dinode* dinode,
                                    size_t block, struct dirent* dirents)
{
  read_inode_block(fs, dinode, block, dirents);

  for (size_t i = 0; i < BSIZE / sizeof(struct dirent); ++i) {
    struct dirent* de = &dirents[i];
    de->inum = ktohs(de->inum);
  }
}

static void cat_file(FILE* fs, const struct superblock* sb, uint inum)
{
  struct dinode inode;
  read_dinode(fs, sb, inum, &inode);

  if (inode.type != T_FILE) {
    fprintf(stderr, "Cannot cat non-file inode of type %u\n", inode.type);
    exit(EXIT_FAILURE);
  }

  if (inode.size > NDIRECT * BSIZE) {
    fprintf(stderr, "Indirect blocks not supported\n");
    exit(EXIT_FAILURE);
  }

  size_t remaining = inode.size;
  size_t block_num = 0;

  while (remaining > 0) {
    char buf[BSIZE];
    read_block(fs, inode.addrs[block_num], buf);

    size_t to_write = remaining;

    if (to_write > BSIZE) {
      to_write = BSIZE;
    }

    if (fwrite(buf, 1, to_write, stdout) != to_write) {
      perror("fwrite");
      exit(EXIT_FAILURE);
    }

    remaining -= to_write;
    ++block_num;
  }
}

int main(int argc, char** argv)
{
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <fs.img> <filename>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char* fs_path = argv[1];
  const char* file_path = argv[2];

  FILE* fs = fopen(fs_path, "r");

  if (fs == NULL) {
    perror("fopen");
    return EXIT_FAILURE;
  }

  struct superblock sb;
  read_sb(fs, &sb);

  if (sb.magic != FSMAGIC) {
    fprintf(stderr, "Invalid file system\n");
    return EXIT_FAILURE;
  }

  struct dinode root;
  read_dinode(fs, &sb, 1, &root);

  for (size_t root_block = 0; root_block < root.size / BSIZE; ++root_block) {
    struct dirent root_entries[BSIZE / sizeof(struct dirent)];
    read_inode_dirent_block(fs, &root, root_block, root_entries );

    for (size_t i = 0; i < BSIZE / sizeof(struct dirent); ++i) {
      struct dirent* de = &root_entries[i];

      if (de->inum != 0 && strcmp(de->name, file_path) == 0) {
        cat_file(fs, &sb, de->inum);
        return EXIT_SUCCESS;
      }
    }
  }

  fprintf(stderr, "File system %s does not contain file %s", fs_path, file_path);
  return EXIT_FAILURE;
}
