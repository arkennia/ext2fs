#include "stat.h"
int local_stat(char *pathname){
   if (strlen(pathname) == 0) {
        printf("No specified pathname!\n");
        return 0;
    }
    int ino = getino(pathname);
    if (getino(pathname) == 0) {
            printf("File name does not exists.\n");
            return 0;
    }
    MINODE *mip = iget(dev, ino);
    printf("File name: %s\n", pathname);
    printf("Mode: %d\n", mip->INODE.i_mode);
    if(mip->INODE.i_mode == 0x41ED){
        printf("Type: Directory\n");
    }
    else if(mip->INODE.i_mode == 0x81A4){
        printf("Type: File\n");
    }
    else if(mip->INODE.i_mode == 0xA1FF){
        printf("Type: Symlink\n");
    }
    if(S_ISLNK(mip->INODE.i_mode)){
        printf("Is link\n");
    }
    printf("Size: %d\n", mip->INODE.i_size);
    printf("Blocks: %d\n", mip->INODE.i_blocks);
    printf("Links: %d\n", mip->INODE.i_links_count);
    printf("Dev: %d\n", mip->dev);
    printf("Ino: %lu\n", mip->ino);
    printf("UID: %d\n", mip->INODE.i_uid);
    //guid and time
    iput(mip);
    return 0;
}
