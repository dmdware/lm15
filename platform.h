


/*******************************************************
 * Copyright (C) 2015 DMD 'Ware <dmdware@gmail.com>
 *
 * This file is part of States, Firms, & Households.
 *
 * You are entitled to use this source code to learn.
 *
 * You are not entitled to duplicate or copy this source code
 * into your own projects, commercial or personal, UNLESS you
 * give credit.
 *
 *******************************************************/




#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
#define PLATFORM_GL14
#define PLATFORM_WIN
#endif

#if __APPLE__

#include "TargetConditionals.h"
#if TARGET_OS_MAC
#define PLATFORM_GL14
#define PLATFORM_MAC
#endif
#if TARGET_OS_IPHONE
#define PLATFORM_IOS
#define PLATFORM_IPHONE
#define PLATFORM_MOBILE
#define PLATFORM_GLES20
#undef PLATFORM_GL41
#endif
#if TARGET_OS_IPAD
#define PLATFORM_IOS
#define PLATFORM_IPAD
#define PLATFORM_MOBILE
#define PLATFORM_GLES20
#undef PLATFORM_GL14
#endif

#endif

#if defined( __GNUC__ )
//#define PLATFORM_LINUX
#endif
#if defined( __linux__ )
#define PLATFORM_LINUX
#define PLATFORM_GL14
#endif
#if defined ( __linux )
#define PLATFORM_LINUX
#define PLATFORM_GL14
#endif

#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#ifdef PLATFORM_WIN
#include <winsock2.h>	// winsock2 needs to be included before windows.h
#include <windows.h>
#include <mmsystem.h>
#include <commdlg.h>
//#include <dirent.h>
#include <Psapi.h>	//mem
#endif

#ifdef PLATFORM_LINUX
/* POSIX! getpid(), readlink() */
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
//file listing dirent
#include <dirent.h>
//htonl
#include <arpa/inet.h>
#include <sys/time.h>
#endif

#if defined(PLATFORM_MAC) && !defined(PLATFORM_IOS)
#include <sys/types.h>
#include <sys/dir.h>
//htonl
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#endif

#if defined(PLATFORM_IOS)
#include <sys/types.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>	//mkdir
#endif

#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <fstream>
#include <iostream>
#include <math.h>
//#ifndef PLATFORM_WIN
//already included in sdl2
#include <stdint.h>
//#endif
#include <limits.h>
#include <unordered_map>

#include <map>

#include <algorithm>    // std::sort

#define DMD_MAX_PATH		123456

#ifndef PLATFORM_WIN
#define _isnan isnan
#endif

#ifdef PLATFORM_WIN
#include <xlocale>
#include "dirent.h"
#else
#include <dirent.h>
#endif

#ifdef PLATFORM_LINUX
#include <SDL2/SDL.h>

#include <jpeglib.h>
#include <png.h>
//#include <zip.h>
#endif

#ifdef PLATFORM_WIN

#include <jpeglib.h>
#include <png.h>
//#include <zip.h>
#endif


#ifdef PLATFORM_WIN
//#include <GL/wglew.h>
#include <SDL.h>
#ifndef MATCHMAKER
//#include <SDL_opengl.h>
//#include <SDL_mixer.h>
//#include <SDL_ttf.h>
#include <SDL_syswm.h>
#endif
//#include <SDL_net.h>
#endif

#ifdef PLATFORM_WIN
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
//#pragma comment(lib, "x64/SDL2.lib")
//#pragma comment(lib, "x64/SDL2main.lib")
#pragma comment(lib, "libjpeg.lib")
#pragma comment(lib, "libpng15.lib")
#endif

#if 0

#undef min
#undef max

extern "C" {
#define __STDC_CONSTANT_MACROS
//#include <stdint.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
	
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/avcodec.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
};

// compatibility with newer API
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif
#endif

#define ERROR 1
typedef unsigned char byte;
#define WORD uint16_t

#ifndef PLATFORM_WIN
#define stricmp strcasecmp
#endif

#ifdef PLATFORM_WIN
#define _isnan(x) (x!=x)
#endif

#define TITLE	"localmap"
#define APPVERSION	14

#endif // #define LIBRARY_H
