/*********** util.c file ****************/

#include "util.h"
#include "globals.h"

#include <ext2fs/ext2_fs.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

// MINODE minode[NMINODE];
// PROC proc[NPROC], *running;
char line[256];
char cmd[64];
char pathname[256];
char cwd[256];
char gline[256]; // holds token strings, each pointed by a name[i]

int get_block(int dev, int blk, char *buf)
{
        lseek(dev, (long)blk * BLKSIZE, 0);
        read(dev, buf, BLKSIZE);
        return 0;
}

int put_block(int dev, int blk, char *buf)
{
        lseek(dev, (long)blk * BLKSIZE, 0);
        write(dev, buf, BLKSIZE);
        return 0;
}

int tokenize(char *pathname)
{
        // copy pathname into gpath[]; tokenize it into name[0] to name[n-1]
        // Code in Chapter 11.7.2
        char *s;
        strcpy(gline, pathname);
        nname = 0;
        s = strtok(gline, "/");
        while (s) {
                name[nname++] = s;
                s = strtok(0, "/");
        }
        return 0;
}

MINODE *iget(int dev, int ino)
{
        // return minode pointer of loaded INODE=(dev, ino)
        // Code in Chapter 11.7.2
        MINODE *mip;
        INODE *ip;
        int i, block, offset;
        char buf[BLKSIZE];
        // serach in-memory minodes first
        for (i = 0; i < NMINODE; i++) {
                mip = &minode[i];
                if (mip->refCount && (mip->dev == dev) && (mip->ino == ino)) {
                        mip->refCount++;
                        return mip;
                }
        }

        // needed INODE=(dev,ino) not in memory
        mip = mialloc();
        mip->dev = dev;
        mip->ino = ino; // assign to (dev, ino)
        block = (ino - 1) / 8 + inode_start; // disk block containing this inode
        offset = (ino - 1) % 8; // which inode in this block
        get_block(dev, block, buf);
        ip = (INODE *)buf + offset;
        mip->INODE = *ip; // copy inode to minode.INODE
        // initialize minode
        mip->refCount = 1;
        mip->mounted = 0;
        mip->dirty = 0;
        mip->mntPtr = 0;
        // printf("Inside iget: ino: %d, block: %d, offset:%d\n", ino, block, offset);
        return mip;
}

void iput(MINODE *mip)
{
        // dispose of minode pointed by mip
        // Code in Chapter 11.7.2
        INODE *ip;
        int block, offset;
        char buf[BLKSIZE];
        if (mip == 0)
                return;
        mip->refCount--; // dec refCount by 1
        if (mip->refCount > 0)
                return; // still has user
        if (mip->dirty == 0)
                return; // no need to write back
        // write INODE back to disk
        block = (mip->ino - 1) / 8 + inode_start;
        offset = (mip->ino - 1) % 8;
        // get block containing this inode
        get_block(mip->dev, block, buf);
        ip = (INODE *)buf + offset; // ip points at INODE
        *ip = mip->INODE; // copy INODE to inode in block
        put_block(mip->dev, block, buf); // write back to disk
        mip->dirty = 0;
}

int search(MINODE *mip, char *name)
{
        // search for name in (DIRECT) data blocks of mip->INODE
        // return its ino
        // Code in Chapter 11.7.2
        int i;
        char *cp, temp[256], sbuf[BLKSIZE];
        DIR *dp;
        for (i = 0; i < 12; i++) { // search DIR direct blocks only
                if (mip->INODE.i_block[i] == 0)
                        return 0;
                get_block(mip->dev, mip->INODE.i_block[i], sbuf);
                dp = (DIR *)sbuf;
                cp = sbuf;
                while (cp < sbuf + BLKSIZE) {
                        strncpy(temp, dp->name, dp->name_len);
                        temp[dp->name_len] = 0;
                        printf("%8d%8d%8u %s\n", dp->inode, dp->rec_len, dp->name_len, temp);
                        if (strcmp(name, temp) == 0) {
                                printf("found %s : inumber = %d\n", name, dp->inode);
                                return dp->inode;
                        }
                        cp += dp->rec_len;
                        dp = (DIR *)cp;
                }
        }
        return 0;
}

int getino(char *pathname)
{
        // return ino of pathname
        // Code in Chapter 11.7.2
        MINODE *mip;
        int i, ino;
        if (strcmp(pathname, "/") == 0) {
                return 2; // return root ino=2
        }
        if (pathname[0] == '/')
                mip = root; // if absolute pathname: start from root
        else
                mip = running->cwd; // if relative pathname: start from CWD
        mip->refCount++; // in order to iput(mip) later
        tokenize(pathname); // assume: name[ ], nname are globals
        for (i = 0; i < nname; i++) { // search for each component string
                if (!S_ISDIR(mip->INODE.i_mode)) { // check DIR type
                        printf("%s is not a directory\n", name[i]);
                        iput(mip);
                        return 0;
                }
                ino = search(mip, name[i]);
                if (!ino) {
                        printf("no such component name %s\n", name[i]);
                        iput(mip);
                        return 0;
                }
                iput(mip); // release current minode
                mip = iget(dev, ino); // switch to new minode
        }
        iput(mip);
        return ino;
}

int findmyname(MINODE *parent, u32 myino, char *myname)
{
        // WRITE YOUR code here:
        // search parent's data block for myino;
        // copy its name STRING to myname[ ];
        char buf[1024];
        INODE inode = parent->INODE;
        char *current;
        DIR *directory;
        u32 block;
        if (inode.i_mode == 16877) {
                for (int i = 0; i < 12; i++) {
                        block = inode.i_block[i];
                        if (block != 0) {
                                get_block(parent->dev, block, buf);
                                current = buf; // copying buf into current
                                directory = (DIR *)buf;
                                while (current < &buf[BLKSIZE]) {
                                        if (directory->inode == myino) {
                                                // coppying name if correct one we want
                                                strcpy(myname, directory->name);
                                                printf("Found.\n");
                                                return 0;
                                        }
                                        // add current
                                        current += directory->rec_len;
                                        directory = (DIR *)current;
                                }
                        } else {
                                printf("Block is 0\n");
                        }
                }
                return 0;
        }
        printf("No directory found\n");
        return 0;
}

int findino(MINODE *mip, u32 *myino) // myino = ino of . return ino of ..
{
        // mip->a DIR minode. Write YOUR code to get mino=ino of .
        //                                         return ino of ..
        char buf[BLKSIZE];
        get_block(mip->dev, mip->INODE.i_block[0], buf);
        DIR *temp;
        char *current = buf;
        temp = (DIR *)buf;
        while (current < buf + BLKSIZE) {
                if (strcmp(temp->name, "..") == 0) {
                        return temp->inode; // returning ..
                }
                if (strcmp(temp->name, ".") == 0) {
                        *myino = temp->inode; // making myino = .
                }
                // advancing current
                current = temp->rec_len + (char *)current;
                temp = (DIR *)current;
        }
        return 0;
}

MINODE *mialloc()
{
        MINODE *mp = NULL;
        for (int i = 0; i < NMINODE; i++) {
                mp = &minode[i];
                if (mp->refCount == 0) {
                        mp->refCount = 1;
                        return mp;
                }
        }
        printf("No free nodes\n");
        return NULL;
}

int midalloc(MINODE *mip)
{
        mip->refCount = 0;
        return 0;
}
int decFreeInodes(int dev)
{
        char buf[BLKSIZE];
        // dec free inodes count in SUPER and GD
        get_block(dev, 1, buf);
        sp = (SUPER *)buf;
        sp->s_free_inodes_count--;
        put_block(dev, 1, buf);
        get_block(dev, 2, buf);
        gp = (GD *)buf;
        gp->bg_free_inodes_count--;
        put_block(dev, 2, buf);
        return 0;
}
int tst_bit(char *buf, int bit)
{
        return buf[bit / 8] & (1 << (bit % 8));
}
int set_bit(char *buf, int bit)
{
        buf[bit / 8] |= (1 << (bit % 8));
        return 0;
}

int ialloc(int dev)
{
        int i;
        char buf[BLKSIZE];
        // use imap, ninodes in mount table of dev
        get_block(dev, imap, buf);
        for (i = 0; i < ninodes; i++) {
                if (tst_bit(buf, i) == 0) {
                        set_bit(buf, i);
                        put_block(dev, imap, buf);
                        // update free inode count in SUPER and GD
                        decFreeInodes(dev);
                        return (i + 1);
                }
        }
        return 0; // out of FREE inodes
}
int balloc(int dev)
{
        int i;
        char buf[BLKSIZE];
        get_block(dev, bmap, buf);
        for (i = 0; i < ninodes; i++) {
                if (tst_bit(buf, i) == 0) {
                        set_bit(buf, i);
                        decFreeInodes(dev);
                        put_block(dev, bmap, buf);
                        return (i + 1);
                }
        }
        return 0;
}

void clr_bit(char *buffer, int bit)
{
        buffer[bit / 8] &= ~(1 << (bit % 8));
}

void incFreeInodes(int dev)
{
        char buffer[BLKSIZE];
        get_block(dev, 1, buffer); // Get super block
        sp = (SUPER *)buffer;
        sp->s_free_inodes_count += 1; // Increment count.
        put_block(dev, 1, buffer);

        memset(buffer, 0, BLKSIZE); // Clear buffer.
        get_block(dev, 2, buffer); // Get group descriptor.
        gp = (GD *)buffer;
        gp->bg_free_inodes_count += 1;
        put_block(dev, 2, buffer);
}

void idalloc(int dev, int ino)
{
        char buffer[BLKSIZE];
        if (ino > ninodes) {
                printf("inumber %d out of range.\n", ino);
                return;
        }
        get_block(dev, imap, buffer);
        clr_bit(buffer, ino - 1);
        put_block(dev, imap, buffer);
        incFreeInodes(dev);
}

void incFreeBlocks(int dev)
{
        char buffer[BLKSIZE];
        get_block(dev, 1, buffer); // Get super block
        sp = (SUPER *)buffer;
        sp->s_free_blocks_count += 1; // Increment count.
        put_block(dev, 1, buffer);

        memset(buffer, 0, BLKSIZE); // Clear buffer.
        get_block(dev, 2, buffer); // Get group descriptor.
        gp = (GD *)buffer;
        gp->bg_free_blocks_count += 1;
        put_block(dev, 2, buffer);
}

void bdalloc(int dev, int bno)
{
        char buffer[BLKSIZE];
        if (bno > nblocks) {
                printf("bnumber %d out of range.\n", bno);
                return;
        }
        get_block(dev, bmap, buffer);
        clr_bit(buffer, bno - 1);
        put_block(dev, bmap, buffer);
        incFreeInodes(dev);
}
