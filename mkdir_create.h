#ifndef MKDIR_CREATE_H
#define MKDIR_CREATE_H
#include <libgen.h>
#include "globals.h"
#include "type.h"
typedef unsigned int u32;

int mkdir_local(char* pathname);
int kmdir(MINODE *pmip, char *basename);
int enter_child(MINODE *pip, int ino, char *name);

#endif
