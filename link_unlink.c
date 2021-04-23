#include "link_unlink.h"
#include "mkdir_create.h"

int link(char *new_file, char *old_file) {
    //verify old_file exists and is not a DIR;
    int oino = getino(old_file);
    MINODE *omip = iget(dev, oino);
    if(strcmp(new_file, "") == 0){
        printf("No new file.\n");
    }
    if(strcmp(old_file, "") == 0){
        printf("No old file.\n");
    }
    if (S_ISDIR(omip->INODE.i_mode))
    {
        printf("Must not be directory type.\n");
        return 0;
    }
    //new_file must not exist yet
    int ino;
    ino = getino(new_file);
    if (ino != 0)
    {
        printf("File already exists\n");
        return 0;
    }
    //create new_file with the same inode number of old_file
    char *parent = dirname(new_file);
    char *child = basename(new_file);
    int pino = getino(parent);
    MINODE *pmip = iget(dev, pino);
    if(!pmip){
        printf("This file has no parent.\n");
    }
    //create entry in new parent DIR with same inode number of old_file
    enter_name(pmip, oino, child);
    omip->INODE.i_links_count++; //increment INODE'S links_count by 1
    omip->dirty = 1;             //for write back by iput(omip)
    iput(omip);
    iput(pmip);
}

