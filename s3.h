#ifndef __s3_h__
#define __s3_h__

#ifdef _WIN32
    #define __PC__
    #define _CRT_SECURE_NO_WARNINGS
    #define  CELL_T int32_t
    #define  UCELL_T uint32_t
#elif _LINUX
    #define __PC__
    #define  CELL_T int64_t
    #define  UCELL_T uint64_t
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifndef CELL_T
#define CELL_T long
#define UCELL_T unsigned long
#endif

#define btw(a,b,c) ((b<=a) && (a<=c))
#define STI(x)   st.i[x]
#define STF(x)   st.f[x]
#define STB(x)   stb[x]
#define TOS      STI(s)
#define NOS      STI(s-1)
#define FTOS     STF(s)
#define FNOS     STF(s-1)
#define PUSH(x)  STI(++s)=(cell_t)(x)
#define POP      STI(s--)
#define R0       STI(r)
#define R1       STI(r+1)
#define R2       STI(r+2)
#define L0       lstk[lsp]
#define L1       lstk[lsp-1]
#define L2       lstk[lsp-2]

// NOTE: change these for your application
#ifdef __PC__
    #define CODE_SZ        (1024*1024)
    #define VARS_SZ        (1024*1024)
    #define FILE_SZ             10
    #define LOCS_SZ             90
    #define LOOP_SZ             30
    #define MAX_FN          0x0FFF
#else // DEV BOARD
    // TEENSY4 likes these: 64k/64k/10/0x0FFF
    // PICO likes these:    48k/32k/10/0x07FF
    // XIAO likes these:    16k/ 2k/10/0x03FF
    #define CODE_SZ        (48*1024)
    #define VARS_SZ        (32*1024)
    #define FILE_SZ             10
    #define LOCS_SZ             90
    #define LOOP_SZ             30
    #define MAX_FN          0x07FF
    #define mySerial        Serial
    #define __FILES__            2
    // #define __GAMEPAD__
#endif

typedef CELL_T cell_t;
typedef UCELL_T ucell_t;
typedef union { float f[VARS_SZ]; cell_t i[VARS_SZ]; } ST_T;

#ifdef __PC__
    #define printStringF printf
    void putC(int c) { putc(c, stdout); }
    void printString(const char *s) { fputs(s, stdout); }
    cell_t doUser(cell_t ir, cell_t pc) { return pc; }
    cell_t doFopen(const char *fn, int mode) { return (cell_t)fopen(fn, mode?"wb":"rb"); }
    void doFclose(cell_t fh) { fclose((FILE*)fh); }
    char *doFgets(char *buf, int sz, cell_t fh) { return fgets(buf, sz, (FILE*)fh); }
    int doFread(void *buf, int sz, int num, cell_t fh) { return fread(buf, sz, num, (FILE*)fh); }
    int doFwrite(void *buf, int sz, int num, cell_t fh) { return fwrite(buf, sz, num, (FILE*)fh); }
    void doFdelete(const char* fn) { remove(fn); }
    #ifdef _LINUX
        cell_t timerMS() { return clock() / 1000; }
        cell_t timerNS() { return clock(); }
        void doFList() { system("ls -l"); }
        int getC() { return fgetc(stdin); }
        int charAvailable() { return 0; }
    #else
        #include <conio.h>
        int charAvailable() { return _kbhit(); }
        int getC() { return _getch(); }
        void doFList() { system("dir"); }
        cell_t timerMS() { return clock(); }
        cell_t timerNS() { return clock() * 1000; }
    #endif
#else
    // It's a board ...
    extern cell_t doFopen(const char *fn, int mode);
    extern void doFclose(cell_t fh);
    extern char *doFgets(char *buf, int sz, cell_t fh);
    extern int doFread(void *buf, int sz, int num, cell_t fh);
    extern int doFwrite(void *buf, int sz, int num, cell_t fh);
    extern void doFList();
    extern void doFdelete(const char* fn);
    extern void init(int files);
    extern void Run(int start);
    extern int charAvailable();
    extern void fDotS();
    extern int getC();
    extern void putC(int);
    extern void printString(const char *str);
    extern void printStringF(const char *fmt, ...);
    extern cell_t doUser(cell_t ir, cell_t pc);
    extern cell_t timerMS();
    extern cell_t timerNS();

    extern uint8_t stb[];
    extern cell_t s, h;
    extern ST_T st;
#endif // __PC__

#endif // __s3_h__
