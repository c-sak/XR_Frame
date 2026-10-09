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
//---- ignore annoying warnings
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
#define PLATFORM_WINDOWS
//----
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <WinSock.h>
#include <math.h>
#include "gl/glut.h"
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
