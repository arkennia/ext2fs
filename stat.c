#include "stat.h"
int local_stat(char *pathname)
{
        //checking pathname is correct
        if (strlen(pathname) == 0) {
                printf("No specified pathname!\n");
                return 0;
        }
        int ino = getino(pathname);
        if (getino(pathname) == 0) {
                printf("File name does not exist.\n");
                return 0;
        }
        MINODE *mip = iget(dev, ino);
        printf("File name: %s\n", pathname);
        printf("Mode: %d\n", mip->INODE.i_mode);
        if (mip->INODE.i_mode == 0x41ED) {
                printf("Type: Directory\n");
        } else if (mip->INODE.i_mode == 0x81A4) {
                printf("Type: File\n");
        } else if (mip->INODE.i_mode == 0xA1FF) {
                printf("Type: Symlink\n");
        }
        if (S_ISLNK(mip->INODE.i_mode)) {
                printf("Is link\n");
        }
        //printing out contents
        printf("Size: %d\n", mip->INODE.i_size);
        printf("Blocks: %d\n", mip->INODE.i_blocks);
        printf("Links: %d\n", mip->INODE.i_links_count);
        printf("Dev: %d\n", mip->dev);
        printf("Ino: %d\n", mip->ino);
        printf("UID: %d\n", mip->INODE.i_uid);
        printf("GID: %d\n", mip->INODE.i_gid);
        printf("Access Time: %s", ctime(&mip->INODE.i_atime));
        printf("Change Time: %s", ctime(&mip->INODE.i_ctime));
        printf("Modify Time: %s", ctime(&mip->INODE.i_mtime));
        iput(mip);
        return 0;
}
