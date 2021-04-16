#ifndef CD_LS_PWD_H
#define CD_LS_PWD_H

#include "util.h"

#define DIR_MODE_INO 4

int chdir(char *pathname);
int ls_file(MINODE *mip, char *name);
int ls_dir(MINODE *mip);
int ls(char *pathname);
int pwd(MINODE *wd);
int rpwd(MINODE *wd, char *buffer);
int is_dir(INODE *ino);

#endif // CD_LS_PWD_H
