// ht - Hash Test program

// NB: the Arduino IDE is STUPID
#define __IS_ARDUINO__

#ifndef __IS_ARDUINO__

#include <stdio.h>
#include <time.h>

#define bk 2047 // # buckets

unsigned short doHash1(const char *v) {
    int h = *(v++);
    while (*v) { h=(33*h)+*(v++); }
    return h & bk;
}

unsigned short doHash2(const char *v) {
    int h = *(v++);
    while (*v) { h = ((h<<3)^h)+(*(v++)); }
    return h & bk;
}

unsigned short doHash3(const char *v) {
    unsigned short x;
    unsigned short crc = 0xFFFF;

    while (*v){
        x = crc >> 8 ^ *(v++);
        x ^= x>>4;
        crc = (crc<<8)^(x<<12)^(x<<5)^(x);
    }
    return crc & bk;
}

unsigned short doHash4(const char *v) {
    // gen seed
    long s = 0;
    for (int i=0; v[i]; i++) { s = (s*31)+v[i]; }
    while (*(v++)) {
        s = (s<<13)^s;
        s = (s>>17)^s;
        s ^= *v;
    }
    return s & bk;
}

static unsigned long sd = 0;
int rand(int lo, int hi) {
    if (sd == 0) { sd = (long)&sd+(long)rand; }
    sd = (sd<<13)^sd;
    sd = (sd>>17)^sd;
    sd = (sd<<5)^sd;
    return sd%(hi-lo)+lo;
}

char x[16];
char *randStr() {
    int l = rand(2,9);
    for (int i=0; i<l; i++) { x[i]=rand('A','Z'); }
    x[l]=0;
    return x;
}

unsigned short tbl[bk+1];

int doTest(int t, int n) {
    int h, coll = 0;
    for (int i=0; i<(bk+1); i++) { tbl[i]=0; }
    for (int i=0; i<n; i++) {
        char *s = randStr();
        if (t==1) { h=doHash1(s); }
        else if (t==2) { h=doHash2(s); }
        else if (t==3) { h=doHash3(s); }
        else if (t==4) { h=doHash4(s); }
        if (tbl[h]!=0) { ++coll; }
        // printf("%s - (%u): %c\n",s,h,(tbl[h]!=0)?'*':' ');
        tbl[h]=1;
    }
    return coll;
}

void doTests(int t, int ns) {
    int tot = 0;
    int nt = 10000;
    int st = clock();
    for (int k=0; k<nt; k++) {
        int coll = doTest(t, ns);
        tot += coll;
        // printf("%d ", coll);
    }
    int e = clock();
    printf("test: %d, %d strings, %d avg collisions (%d)\n", t, ns, tot/nt,(e-st)/1000);
}

int main() {
    for (int i=0; i<10; i++) {
        int ns = rand(250, 350);
        doTests(1, ns);
        doTests(2, ns);
        doTests(3, ns);
        doTests(4, ns);
        printf("\n");
        // doTests(4, ns);
    }
}
#endif
