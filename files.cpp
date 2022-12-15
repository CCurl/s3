// File system support for the development boards

#include "s3.h"

#if __FILES__ == 0 // No file support

cell_t doFopen(const char* fn, int mode) { return 0; }
void doFclose(cell_t fh) {}
char* doFgets(char* buf, int sz, cell_t fh) { return 0; }
int doFread(void* buf, int sz, int num, cell_t fh) { return 0; }
int doFwrite(void* buf, int sz, int num, cell_t fh) { return 0; }
void doFList() {}
void doFdelete(const char* fn) {}

#elif  __FILES__ == 2 // TEENSY4

#include "file-teensy.h"

#elif  __FILES__ == 3 // PICO

cell_t doFopen(const char* fn, int mode) { return 0; }
void doFclose(cell_t fh) {}
char* doFgets(char* buf, int sz, cell_t fh) { return 0; }
int doFread(void* buf, int sz, int num, cell_t fh) { return 0; }
int doFwrite(void* buf, int sz, int num, cell_t fh) { return 0; }
void doFList() {}
void doFdelete(const char* fn) {}

#endif
