
#define MAX_FILES 10
File files[MAX_FILES + 1];
int numFiles = 0;
File f;

/*
cell_t doFopen(const char *fn, int mode) { return 0; }
void doFclose(cell_t fh) {}
int doFread(void *buf, int sz, int num, cell_t fh) { return 0; }
int doFwrite(void *buf, int sz, int num, cell_t fh) { return 0; }
char *doFgets(char *buf, int sz, cell_t fh) { return 0; }
void doFdelete(const char *fn) {}
void doFlist() {}
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

cell_t doFopen(const char *fn, int mode) {
    int fh = 0;
    for (int i = 1; (i<=MAX_FILES) & (fh==0); i++) {
        if (!files[i]) { fh = i; }
    }
    if (fh) {
        f = myFS.open(fn, (mode) ? FILE_WRITE : FILE_READ);
        if (f) { files[fh] = f; }
    }
    else { printString("-fileFull-"); }
    return fh;
}

void doFclose(cell_t fh) {
    if (btw(fh, 1, MAX_FILES) && (files[fh])) {
        files[fh].close();
    }
}

int doFread(void *buf, int sz, int num, cell_t fh) {
    if (btw(fh, 1, MAX_FILES) && (files[fh])) {
        return files[fh].read(buf, sz);
    }
    return 0;
}

int doFwrite(void *buf, int sz, int num, cell_t fh) {
    if (btw(fh, 1, MAX_FILES) && (files[fh])) {
        return files[fh].write(buf, sz);
    }
    return 0;
}

char *doFgets(char *buf, int sz, cell_t fh) {
  *buf = 0;
  if (btw(fh, 1, MAX_FILES) && (files[fh])) {
      *(buf) = ';';
      *(buf+1) = 0;
      return buf;
  }
  return (char *)0; 
}

void doFdelete(const char* fn) {
    myFS.remove(fn);
}

void doFlist() {
}
