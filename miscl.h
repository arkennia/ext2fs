#ifndef MISCL_H
#define MISCL_H

#include "globals.h"

/// Check if the pathname is accessible.
int access_local(char *pathname);
/// Change the mode of the file.
void chmod_local(char *pathname, int mode);
/// Update the modified time.
void update_mtime(char *pathname);
/// Update the accessed time.
void update_atime(char *pathname);
/// Update the afiles time fields.
void utime_local(char *pathname);

#endif // MISCL_H
