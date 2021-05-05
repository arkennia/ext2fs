#include "rmdir.h"

int rmdir_local(char *pathname)
{
        int ino = 0;
        int parent_ino = 0;
        MINODE *mip = NULL;
        MINODE *parent_mip = NULL;
        char name[256];
        u32 tmp;

        // Get inode of pathname in memory.
        ino = getino(pathname);
        mip = iget(dev, ino);

        if (is_valid_directory(mip) == false)
                return -1;
        parent_ino = findino(mip, &tmp);
        parent_mip = iget(mip->dev, parent_ino);

        findmyname(parent_mip, ino, name);
        rm_child(parent_mip, name);

        iput(parent_mip);

        bdalloc(mip->dev, mip->INODE.i_block[0]);
        idalloc(mip->dev, mip->ino);
        iput(mip);

        return 0;
}

void rm_child(MINODE *parent_mip, char *file_name)
{
        INODE *parent = &parent_mip->INODE;
        int tempIno = 0;
        char buf[BLKSIZE];
        char *cp = NULL;
        int block = -1;
        int pos = 0;
        int total_entries = get_num_links(parent_mip);
        DIR *prevDir = NULL;

        for (int i = 0; i < 12; i++) {
                if (parent->i_block[i] == 0) {
                        break;
                }
                // storing i_block[i] into tempIno
                tempIno = parent->i_block[i];
                // step to the last entry in the data block
                get_block(dev, parent->i_block[i], buf);
                dp = (DIR *)buf; // Set dp to first entry in dir (typically .).
                cp = buf;
                while (cp + dp->rec_len <= buf + BLKSIZE) {
                        pos++;
                        if (strcmp(dp->name, file_name) == 0) {
                                block = parent->i_block[i];
                                break;
                        }
                        prevDir = dp;
                        cp += dp->rec_len;
                        dp = (DIR *)cp;
                }
        }
        if (pos == 0) {
                bdalloc(dev, block);
                parent_mip->INODE.i_size = parent_mip->INODE.i_size - BLKSIZE;
                adjust_iblock_array(parent_mip);
        } else if (pos == total_entries) {
                prevDir->rec_len += dp->rec_len;
        } else {
                cp = buf;
                while (cp + dp->rec_len < buf + BLKSIZE) {
                        if (strcmp(dp->name, file_name) == 0) {
                                break;
                        }
                        prevDir = dp;
                        cp += dp->rec_len;
                        dp = (DIR *)cp;
                }
                cp += dp->rec_len;
                memcpy(dp, cp, dp->rec_len);
                prevDir->rec_len += dp->rec_len;
        }
        put_block(dev, tempIno, buf);
}

void adjust_iblock_array(MINODE *parent_mip)
{
        int tmp = 0;
        for (int i = 0; i < 11; i++) {
                if (parent_mip->INODE.i_block[i] == 0 && parent_mip->INODE.i_block[i + 1] > 0) {
                        tmp = parent_mip->INODE.i_block[i];
                        parent_mip->INODE.i_block[i] = parent_mip->INODE.i_block[i + 1];
                        parent_mip->INODE.i_block[i + 1] = 0;
                }
        }
}

bool is_valid_directory(MINODE *mip)
{
        if (!S_ISDIR(mip->INODE.i_mode)) {
                printf(" Error rmdir: Not a directory.\n");
                return false;
        }

        if (mip->refCount > 1) {
                printf("Error rmdir: Directory busy.\n");
                return false;
        }

        if (get_num_links(mip) > 2) {
                printf("Error rmdir: Trying to remove non-empty directory.\n");
                return false;
        }

        return true;
}

int get_num_links(MINODE *mip)
{
        int link_count = 1;
        INODE *parent = &mip->INODE;
        int tempIno = 0;
        char buf[BLKSIZE];
        char *cp = NULL;

        for (int i = 0; i < 12; i++) {
                if (parent->i_block[i] == 0) {
                        break;
                }
                // storing i_block[i] into tempIno
                tempIno = parent->i_block[i];
                // step to the last entry in the data block
                get_block(dev, parent->i_block[i], buf);
                dp = (DIR *)buf; // Set dp to first entry in dir (typically .).
                cp = buf;
                while (cp + dp->rec_len < buf + BLKSIZE) {
                        cp += dp->rec_len;
                        dp = (DIR *)cp;
                        link_count++;
                }
        }

        return link_count;
}
