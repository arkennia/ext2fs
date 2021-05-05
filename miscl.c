#include "miscl.h"
#include "util.h"

void chmod_local(char *pathname, int mode)
{
        printf("chmod mode:%o %s\n", mode, pathname);
        int ino = getino(pathname);
        if (ino == 0) {
                printf("chmod: path does not exist.\n");
                return;
        }
        MINODE *mip = iget(dev, ino);
        mip->INODE.i_mode |= mode;
        mip->INODE.i_ctime = time(NULL);
        mip->dirty = 1;
        iput(mip);
}

void update_ctime(char *pathname)
{
        int ino = getino(pathname);
        if (ino == 0)
                return;
        MINODE *mip = iget(dev, ino);
        mip->INODE.i_ctime = time(NULL);
        mip->dirty = 1;
        iput(mip);
}

void update_mtime(char *pathname)
{
        int ino = getino(pathname);
        if (ino == 0)
                return;
        MINODE *mip = iget(dev, ino);
        mip->INODE.i_mtime = time(NULL);
        mip->dirty = 1;
        iput(mip);
}

void update_atime(char *pathname)
{
        int ino = getino(pathname);
        if (ino == 0)
                return;
        MINODE *mip = iget(dev, ino);
        mip->INODE.i_atime = time(NULL);
        mip->dirty = 1;
        iput(mip);
}

void utime_local(char *pathname)
{
        int ino = getino(pathname);
        if (ino == 0)
                return;
        MINODE *mip = iget(dev, ino);
        mip->INODE.i_atime = time(NULL);
        mip->INODE.i_mtime = time(NULL);
        mip->dirty = 1;
        iput(mip);
}
