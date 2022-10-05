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

// NOTE: change these  for your application
#define CODE_SZ        (64*1024)
#define VARS_SZ        (64*1024)
#define FILE_SZ             10
#define MAX_FN          0x07FF

typedef unsigned char BYTE;
typedef union { float f[VARS_SZ]; long i[VARS_SZ]; } ST_T;

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifdef _WIN32
	#define __PC__
	int  getC() { return fgetc(stdin); }
	void putC(int c) { putc(c, stdout); }
	void printString(const char *s) { printf(s); }
	long doUser(long ir, long pc) { return pc; }
#elif _LINUX
	#define __PC__
	int getC() { return fgetc(stdin); }
	void putC(int c) { putc(c, stdout); }
	void printString(const char *s) { printf(s); }
	long doUser(long ir, long pc) { return pc; }
#else
	// It's a board ...
	// #define _TEENSY4_
	// #define _XIAO_
	#define _PICO_
	// #define _FILES_
	// #define __GAMEPAD__
	#define putC(c)         printChar(c)
	#define printString(s)  printSerial(s)
	extern void init(int);
	extern void Run(int);
	extern int printStringF(const char *fmt, ...);
	extern int charAvailable();
	extern void fDotS();
	extern int getC();
	extern void putC(int);
	extern void printString(const char *);
	extern long doUser(long, long);

	extern BYTE stb[];
	extern long s, h;
	extern ST_T st;
#endif

#endif // __s3_h__
