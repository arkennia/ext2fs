#ifndef MISCL_H
#define MISCL_H

#include "globals.h"

/// Change the mode of the file.
void chmod_local(char *pathname, int mode);
/// Update the changed time.
void update_ctime(char *pathname);
/// Update the modified time.
void update_mtime(char *pathname);
/// Update the accessed time.
void update_atime(char *pathname);
/// Update the afiles time fields.
void utime_local(char *pathname);

#endif // MISCL_H
