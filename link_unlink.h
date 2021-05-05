#ifndef LINK_UNLINK_H
#define LINK_UNLINK_H
#include "globals.h"

int link(char *old_file, char *new_file);
int unlink(char *pathname);
// int rm_child(MINODE *pmip, char *name);
int symlink(char *old_file, char *new_file);
int readLink(char *pathname, char *buffer);
#endif
