#include "mkdir_create.h"
#include "util.h"

int mkdir_local(char *pathname)
{
        char *base;
        int ino;
        if (strlen(pathname) == 0) {
                printf("No specified pathname!\n");
                return 0;
        }
        char patnameTemp[BLKSIZE];
        strcpy(patnameTemp, pathname);
        // seperating basename and parentname
        char *parent = dirname(pathname);
        base = basename(patnameTemp);
        ino = getino(parent);
        if (ino == 0) {
                printf("Not a valid path.\n");
                return 0;
        }
        // dirname must exist and is a DIR
        int pino = getino(parent);
        MINODE *pip = iget(dev, pino);
        if (getino(pathname) != 0) {
                printf("Directory name exists.\n");
                return 0;
        }
        if (!S_ISDIR(pip->INODE.i_mode)) {
                printf("Not a valid directory name.\n");
                return 0;
        }
        kmkdir(pip, base);
        return 0;
}

int kmkdir(MINODE *pmip, char *basename)
{
        char *cp;
        char buf[BLKSIZE];
        int ino = ialloc(running->cwd->dev);
        int blk = balloc(running->cwd->dev);
        // load INODE into minode
        MINODE *mip = iget(running->cwd->dev, ino);
        // initialize mip->INODE as a DIR INODE;
        ip = &(mip->INODE);
        ip->i_size = BLKSIZE;

        ip->i_atime = time(0L);
        ip->i_ctime = time(0L);
        ip->i_mtime = time(0L);

        ip->i_mode = 0x41ED;

        ip->i_uid = running->uid;
        ip->i_gid = running->gid;

        ip->i_links_count = 2;

        ip->i_blocks = 2;
        // making first i_block blk
        ip->i_block[0] = blk;
        int i = 1;
        mip->dirty = 1;

        // making all other iblocks be 0
        while (i <= 14) {
                ip->i_block[i] = 0;
                i++;
        }
        // write INODE back to disk
        iput(mip);
        // creating . directory. making a data block of 0 of INODE to contain . and ..
        get_block(running->cwd->dev, blk, buf);
        dp = (DIR *)buf;
        cp = buf;
        dp->inode = ino;
        // ideal size
        dp->rec_len = 4 * ((8 + 1 + 3) / 4);
        dp->name_len = 1;
        strcpy(dp->name, ".");
        // creating .. directory
        cp += dp->rec_len; // advancing cp
        dp = (DIR *)cp;

        dp->inode = pmip->ino;
        dp->rec_len = 1012;
        dp->name_len = 2;
        
        strcpy(dp->name, "..");

        put_block(running->cwd->dev, blk, buf);
        // enters ino, basename asd a dir_entry to the parent INODE
        enter_child(pmip, ino, basename, EXT2_FT_DIR);

        return 1;
}

int enter_child(MINODE *mip, int ino, char *name, int type)
{

        int i;
        INODE *parent = &mip->INODE;
        int length = strlen(name);
        char buf[BLKSIZE];
        char *cp;
        DIR *dp;
        int ideal_length;
        int remain;
        int need_length;

        int tempIno = 0;

        // assume there are only 12 directories
        // looping through each data block of parent directory
        for (i = 0; i < 12; i++) {
                if (parent->i_block[i] == 0) {
                        break;
                }
                // storing i_block[i] into tempIno
                tempIno = parent->i_block[i];
                need_length = 4 * ((8 + length + 3) / 4); // a multiple of 4
                // step to the last entry in the data block
                get_block(dev, parent->i_block[i], buf);
                dp = (DIR *)buf;
                cp = buf;
                while (cp + dp->rec_len < buf + BLKSIZE) {
                        cp += dp->rec_len;
                        dp = (DIR *)cp;
                }
                // dp now points at the last entry in block
                cp = (char *)dp;
                ideal_length = 4 * ((8 + dp->name_len + 3) / 4);
                remain = dp->rec_len  - ideal_length; // remain = LAST entry's rec_len - its ideal_length
                if (remain >= need_length) {
                        // enter the new entry as the LAST entry and trim the previous entry
                        // entry rec_len to its ideal_length
                        dp->rec_len = ideal_length;
                        // advancing cp
                        cp += dp->rec_len;
                        dp = (DIR *)cp;
                        dp->rec_len = remain;
                        dp->name_len = length;
                        strcpy(dp->name, name);

                        dp->inode = ino;
                        dp->file_type = type;

                        put_block(dev, tempIno, buf);
                        return 1;
                }
        }

        tempIno = balloc(dev);
        parent->i_block[i] = tempIno;

        parent->i_size += BLKSIZE;
        // making mip dirty
        mip->dirty = 1;
        get_block(dev, tempIno, buf);

        dp = (DIR *)buf;
        cp = buf;

        // creating dp and copying name
        strcpy(dp->name, name);
        dp->inode = ino;
        dp->rec_len = BLKSIZE;
        dp->file_type = type;
        dp->name_len = length;
        put_block(dev, tempIno, buf);

        return 1;
}

// Creates a regular file.
int creat_local(const char *pathname)
{
        char *base;
        int ino;
        if (strlen(pathname) == 0) {
                printf("No specified pathname!\n");
                return 0;
        }
        char pathnameTemp[BLKSIZE];
        strcpy(pathnameTemp, pathname);
        // seperating basename and parentname
        char *parent = dirname(pathnameTemp);
        base = basename(pathnameTemp);
        ino = getino(parent);
        if (ino == 0) {
                printf("Not a valid path.\n");
                return 0;
        }
        // makes sure you'r trying to make this in a directory.
        int pino = getino(parent);
        MINODE *pip = iget(dev, pino);
        if (getino(pathnameTemp) != 0) {
                printf("File name exists.\n");
                return 0;
        }
        if (!S_ISDIR(pip->INODE.i_mode)) {
                printf("Not a valid directory name.\n");
                return 0;
        }
        kcreat(pip, base);
        return 0;
}

int kcreat(MINODE *pmip, char *basePath)
{
        int ino = ialloc(running->cwd->dev);
        // load INODE into minode
        MINODE *mip = iget(running->cwd->dev, ino);
        // initialize mip->INODE as a DIR INODE;
        ip = &(mip->INODE);
        ip->i_size = 0;

        ip->i_atime = time(0L);
        ip->i_ctime = time(0L);
        ip->i_mtime = time(0L);

        ip->i_mode = 0x81A4;

        ip->i_uid = running->uid;
        ip->i_gid = running->gid;

        ip->i_links_count = 2;

        ip->i_blocks = 2;
        // making first i_block blk
//        int i = 1;
        mip->dirty = 1;

        iput(mip);
        // enters ino, basename asd a dir_entry to the parent INODE
        enter_child(pmip, ino, basePath, EXT2_FT_REG_FILE);

        return 1;
}
