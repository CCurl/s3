#ifndef __s3_h__
#define __s3_h__

#define btw(a,b,c) ((b<=a) && (a<=c))
#define TOS      st.i[s]
#define NOS      st.i[s-1]
#define PUSH(x)  st.i[++s]=(long)(x)
#define POP      st.i[s--]
#define R0       st.i[r]
#define R1       st.i[r+1]
#define R2       st.i[r+2]

#ifdef _WIN32
    #define __PC__
#elif _LINUX
    #define __PC__
#endif

// NOTE: change these for your application
#ifdef __PC__
    #define CODE_SZ        (64*1024)
    #define VARS_SZ        (64*1024)
    #define FILE_SZ             10
    #define MAX_FN          0x0FFF
#else // DEV BOARD
    // TEENSY4 likes these: 64k/64k/10/0x0FFF
    // PICO likes these:    48k/32k/10/0x07FF
    // XIAO likes these:    16k/ 2k/10/0x03FF
    #define CODE_SZ        (48*1024)
    #define VARS_SZ        (32*1024)
    #define FILE_SZ             10
    #define MAX_FN          0x07FF
#endif

typedef unsigned char BYTE;
typedef union { float f[VARS_SZ]; long i[VARS_SZ]; } ST_T;

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifdef __PC__
    int getC() { return fgetc(stdin); }
    void putC(int c) { putc(c, stdout); }
    void printString(const char *s) { fputs(s, stdout); }
    long doUser(long ir, long pc) { return pc; }
    long doFopen(const char *fn, int mode) { return (long)fopen(fn, mode?"wb":"rb"); }
    void doFclose(long fh) { fclose((FILE*)fh); }
    char *doFgets(char *buf, int sz, long fh) { return fgets(buf, sz, (FILE*)fh); }
    int doFread(void *buf, int sz, int num, long fh) { return fread(buf, sz, num, (FILE*)fh); }
    int doFwrite(void *buf, int sz, int num, long fh) { return fwrite(buf, sz, num, (FILE*)fh); }
    long timerMS() { return clock(); }
    long timerNS() { return clock() * 1000; }
#else
    // It's a board ...
    // #define _TEENSY4_
    // #define _XIAO_
    #define _PICO_
    #define mySerial Serial
    // #define __FILES__
    // #define __GAMEPAD__
    extern long doFopen(const char *fn, int mode);
    extern void doFclose(long fh);
    extern char *doFgets(char *buf, int sz, long fh);
    extern int doFread(void* buf, int sz, int num, long fh);
    extern int doFwrite(void* buf, int sz, int num, long fh);
    extern void init(int files);
    extern void Run(int start);
    extern int printStringF(const char *fmt, ...);
    extern int charAvailable();
    extern void fDotS();
    extern int getC();
    extern void putC(int);
    extern void printString(const char *str);
    extern long doUser(long ir, long pc);
    extern long timerMS();
    extern long timerNS();

    extern BYTE stb[];
    extern long s, h;
    extern ST_T st;
#endif // __PC__

#endif // __s3_h__
