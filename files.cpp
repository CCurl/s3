// File system support for the development boards

#include "s3.h"

#ifndef FILES_IMPL // No support for files

cell_t doFopen(const char *fn, int mode) { return 0; }
void doFclose(cell_t fh) {}
char *doFgets(char *buf, int sz, cell_t fh) { return 0; }
int doFread(void *buf, int sz, int num, cell_t fh) { return 0; }
int doFwrite(void *buf, int sz, int num, cell_t fh) { return 0; }
void doFlist() {}
void doFdelete(const char *fn) {}

#else

#include FILES_IMPL

#endif
