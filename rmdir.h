#ifndef RMDIR_H
#define RMDIR_H

#include "globals.h"
#include "util.h"
#include <stdbool.h>

/// Removes a directory.
int rmdir_local(char *pathname);

/// Removes the given file_name from the parent.
void rm_child(MINODE *parent_mip, char *file_name);

/// Compact the inode array.
void adjust_iblock_array(MINODE *parent_mip);

/// Checks if the MINODE points to a valid directory.
bool is_valid_directory(MINODE *mip);

/// Gets number of links on mip.
int get_num_links(MINODE *mip);

#endif // RMDIR_H
