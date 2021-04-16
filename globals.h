#ifndef GLOBALS_H
#define GLOBALS_H

#include "type.h"

// globals
extern MINODE minode[NMINODE];
extern MINODE *root;

extern PROC proc[NPROC], *running;

extern char gpath[128]; // global for tokenized components
extern char *name[32]; // assume at most 32 components in pathname
extern int n; // number of component strings

extern int fd, dev;
extern int nblocks, ninodes, bmap, imap, inode_start;

extern SUPER *sp;
extern GD *gp;
extern INODE *ip;
extern DIR *dp;

extern int nname; // number of token strings

#endif // GLOBALS_H
