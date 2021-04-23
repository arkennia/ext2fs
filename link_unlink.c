#include "link_unlink.h"
#include "mkdir_create.h"
#include "rmdir.h"

int link(char *old_file, char *new_file) {
    //verify old_file exists and is not a DIR;
    int oino = getino(old_file);
    MINODE *omip = iget(dev, oino); 
    if(!omip){
        printf("File does not exist\n");
    }
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
    char *temp[BLKSIZE];
    strcpy(temp, new_file);
    char *parent = dirname(new_file);
    char *child = basename(temp);
    //link /linkFile /DIR1/new
    printf("Child: %s", child);
    int pino = getino(parent); 
    MINODE *pmip = iget(dev, pino); 
    if(!pmip){
        printf("This file has no parent.\n");
        return 0;
    }
    if(!S_ISDIR(pmip->INODE.i_mode)){
        printf("Must be a directory.\n");
        return 0;
    }
    //create entry in new parent DIR with same inode number of old_file
    enter_child(pmip, oino, child);
    omip->INODE.i_links_count++; //increment INODE'S links_count by 1
    omip->dirty = 1; 
    pmip->INODE.i_atime = time(0L);
    pmip->dirty = 1;            //for write back by iput(omip)
    iput(omip);
    iput(pmip);
    return 1;
}
int unlink(char *pathname){
    // get filenames's minode
    int ino = getino(pathname);
    MINODE *mip = iget(running->cwd->dev, ino);
    //check it's a REG or symbolic LNK file; can not be a DIR
   if(S_ISDIR(mip->INODE.i_mode)){
       printf("Cannot be a directory");
   }
   char *temp[256];
   //remove name entry from parent DIR's data block
   strcmp(temp, pathname);
   char *parent = dirname(pathname);
   char *child = basename(temp);
   int pino = getino(parent);
   MINODE* pmip = iget(dev, pino);
   //rm_child(pmip, ino, child);
    pmip->dirty = 1;
    iput(pmip);
    //decrement INODE'S link_count by 1
    mip->INODE.i_links_count --;
    if(mip->INODE.i_links_count > 0){
        mip->dirty = 1; //for write INODE back to disk
    }
    else{//if links_count = 0; remove filename
    

    }

}