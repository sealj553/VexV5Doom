#include "unlink.h"

#include <errno.h>

#undef errno
extern int errno;

int _unlink (char *name) {
    errno = ENOENT;
    return -1;
}
