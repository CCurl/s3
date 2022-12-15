#include "LittleFS.h"

#define MAX_FILES 10
File files[MAX_FILES + 1];
int numFiles = 0;
File f;

/*
cell_t doFopen(const char *fn, int mode) { return 0; }
void doFclose(cell_t fh) {}
char *doFgets(char *buf, int sz, cell_t fh) { return 0; }
int doFread(void *buf, int sz, int num, cell_t fh) { return 0; }
int doFwrite(void *buf, int sz, int num, cell_t fh) { return 0; }
void doFList() {}
void doFdelete(const char *fn) {}
*/

LittleFS_Program myFS;
void fileInit() {
    myFS.begin(1 * 1024 * 1024);
    printString("\r\nLittleFS: initialized");
    printStringF("\r\nBytes Used: %llu, Bytes Total:%llu", myFS.usedSize(), myFS.totalSize());
}

int freeFile() {
    for (int i = 1; i <= MAX_FILES; i++) {
        if (!files[i]) { return i; }
    }
    return 0;
}

cell_t doFopen(const char* fn, int mode) {
    int fh = 0;
    for (int i = 1; (i<=MAX_FILES) & (fh==0); i++) {
        if (!files[i]) { fh = i; }
    }
    if (fh) {
        f = myFS.open(fn, (*md == 'w') ? FILE_WRITE : FILE_READ);
        if (f) { files[fh] = f; }
    }
    else { printString("-fileFull-"); }
    return fh;
}

void doFclose(cell_t fh) {
    if (BetweenI(fh, 1, MAX_FILES) && (files[fh])) {
        files[fh].close();
    }
}

void doFdelete(const char *fn) {
    myFS.remove(fn);
}

void doFList() {
}

int doFread(void *buf, int sz, int num, cell_t fh) {
    byte c = 0;
    if (BetweenI(fh, 1, MAX_FILES) && (files[fn])) {
        return files[fn].read(buf, sz);
    }
    return 0;
}

int doFwrite(void *buf, int sz, int num, cell_t fh) {
    if (BetweenI(fh, 1, MAX_FILES) && (files[fh])) {
        return files[fh].write(buf, sz);
    }
}
