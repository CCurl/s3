#ifndef __config_h__
#define __config_h__

#ifdef _WIN32
	#define __PC__
	#define _CRT_SECURE_NO_WARNINGS
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
#include <string.h>

// NOTE: change these for your application
#ifdef __PC__
    #define CODE_SZ        (1024*1024)
    #define VARS_SZ        (1024*1024)
    #define FILE_SZ             10
    #define LOCS_SZ             90
    #define LOOP_SZ             30
    #define MAX_FN          0x0FFF
#else // DEV BOARD
    // TEENSY4 likes these:  64k/64k/10/90/30/0x0FFF/Serial/file-teensy
    // PICO    likes these:  48k/32k/10/90/30/0x07FF/Serial/file-pico
    // XIAO    likes these:  16k/ 2k/ 0/40/15/0x03FF/Serial/
    #define CODE_SZ        (64*1024)
    #define VARS_SZ        (64*1024)
    #define FILE_SZ             10
    #define LOCS_SZ             90
    #define LOOP_SZ             30
    #define MAX_FN          0x0FFF
    #define               NEEDS_ALIGN
    #define mySerial        Serial
    #define FILES_IMPL   "file-teensy.h"
    #include "LittleFS.h"
    // #define __GAMEPAD__
#endif

#ifndef CELL_T
#define  CELL_T int32_t
#define  UCELL_T uint32_t
#endif

typedef CELL_T cell_t;
typedef UCELL_T ucell_t;
typedef union { float f[VARS_SZ]; cell_t i[VARS_SZ]; } ST_T;

#endif // __config_h__
