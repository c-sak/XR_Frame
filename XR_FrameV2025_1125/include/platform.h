#ifndef PLATFORM_H
#define PLATFORM_H
//↑platform.hのみ、教材として
//「インクルードガード表記」の例として残しておきます
//それ以外のヘッダーファイルでは
//#pragma once
//を使うことにします
//-------- common
#define _USE_MATH_DEFINES

//-------- Microsoft Windows OS
#if defined _WIN64 || defined _WIN32

#if 1
#pragma comment(lib, "libjpeg.lib")
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "libpng16.lib")
#pragma comment(lib, "ViconDataStreamSDK_CPP.lib" )
#pragma comment(lib, "freeglut.lib")
#else
#pragma comment(lib, "vs2010\libjpeg.lib")
#pragma comment(lib, "vs2010\zlib.lib")
#pragma comment(lib, "vs2010\libpng16.lib")
#endif

//---- ignore annoying warnings
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
#pragma warning(disable : 4334)
#define PLATFORM_WINDOWS
//----
#include <stdlib.h>
#include <stdio.h>
//#include <WinSock.h>
#include <WinSock2.h>
#include <windows.h>
#include <math.h>

#if defined _WIN64
#include "gl/freeglut.h"
#else
#include "GL/glut.h"
#endif

#include "GL/glext.h"
//#define GL_GLEXT_PROTOTYPES

#include "config.h"
static inline void msleep( int msec ){ Sleep( msec ); }
#else 
//-------- Mac OS
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#define __OSX_AVAILABLE_STARTING(...)
#define __OSX_AVAILABLE_BUT_DEPRECATED_MSG(...)
//#include <stdlib.h>
//#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cmath>
#include <GLUT/glut.h>
#define PLATFORM_MACOS
static inline void msleep( int msec ){ usleep( msec ); }
static inline void Sleep( int msec ){ msleep( msec ); }
#endif

#include "myShape.h"
//-------------------------
#endif
// End of platform.h