#ifndef MKDIR_CREATE_H
#define MKDIR_CREATE_H
#include "globals.h"
#include <libgen.h>

int creat_local(const char *pathname);
int kcreat(MINODE *pmip, char *basePath);
int mkdir_local(char *pathname);
int kmkdir(MINODE *pmip, char *base);
int enter_child(MINODE *pip, int ino, char *name, int type);

#endif
