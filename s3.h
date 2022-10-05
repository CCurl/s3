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

typedef unsigned char BYTE;

#ifdef _WIN32
	#define __PC__
	#define _CRT_SECURE_NO_WARNINGS
	#define CODE_SZ        (64*1024)
	#define VARS_SZ        (64*1024)
	#define FILE_SZ             10
	#define MAX_FN          0x07FF
	#define putC(ch)        putc(ch, stdout)
	#define getC()          fgetc(stdin)
	#define printString(s)  printf(s)
	#define doUser(u,p)     p
#elif _LINUX
	#define __PC__
	#define CODE_SZ        (64*1024)
	#define VARS_SZ        (64*1024)
	#define FILE_SZ             10
	#define MAX_FN          0x07FF
	#define putC(ch)        putc(ch, stdout)
	#define getC()          fgetc(stdin)
	#define printString(s)  printf(s)
	#define doUser(u,p)     p
#else
	// It's a board ...
	#define _TEENSY4_
	// #define _XIAO_
	// #define _PICO_
	// #define _FILES_
	// #define __GAMEPAD__
	#define CODE_SZ         (64*1024)
	#define VARS_SZ         (32*1024)
	#define FILE_SZ             10
	#define MAX_FN          0x07FF
	#define putC(c)         printChar(c)
	#define printString(s)  printSerial(s)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

extern int printStringF(const char *fmt, ...);
extern int charAvailable();
extern int getChar();
extern void fDotS();

extern BYTE stb[];
extern long h;

#endif __s3_h__
