#ifndef _FUSELIB_H_
#define _FUSELIB_H_

#include <fuse.h>
#include "myFS.h"

#define FUSE_USE_VERSION 26
#define MAX_FUSE_NARGS 64

extern struct fuse_operations myFS_operations;

extern MyFileSystem myFileSystem;

#endif
