#ifndef UTIL_H
#define UTIL_H

#include "type.h"
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
typedef unsigned int u32;

int get_block(int dev, int blk, char *buf);
int put_block(int dev, int blk, char *buf);
int tokenize(char *pathname);
MINODE *iget(int dev, int ino);
void iput(MINODE *mip);
int search(MINODE *mip, char *name);
int getino(char *pathname);
int findmyname(MINODE *parent, u32 myino, char *myname);
int findino(MINODE *mip, u32 *myino);
MINODE *mialloc();
int midalloc(MINODE *mip);
int decFreeInodes(int dev);
int tst_bit(char *buf, int bit);
int set_bit(char *buf, int bit);
int ialloc(int dev);
int balloc(int dev);

/// Deallocate an inode.
void idalloc(int dev, int ino);

/// Clear bit in bitmap.
void clr_bit(char *buffer, int bit);

/// Increment number of free inodes.
void incFreeInodes(int dev);

/// Dealloactes a disk block.
void bdalloc(int dev, int bno);

/// Increment the number of free blocks.
void incFreeBlocks(int dev);

#endif // UTIL_H
