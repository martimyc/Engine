#ifndef _GLOBALS
#define _GLOBALS

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 ) // Warning that exceptions are disabled

#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) _log(__FILE__, __LINE__, format, __VA_ARGS__)

void _log(const char file[], int line, const char* format, ...);

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))
#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI

typedef unsigned int uint;

enum UPDATE_STATUS
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

#define DELETE_PTR( x )		\
{							\
	if (x != nullptr)		\
	{						\
		delete x;			\
		x = nullptr;		\
	}						\
}							\

#define DELETE_ARRAY( x )	\
{							\
	if (x != nullptr)		\
	{						\
		delete []x;			\
		x = nullptr;		\
	}						\
}							\

//openGL context versios in use
#define OPENGL_CONTEXT_MINOR_VERSION 1
#define OPENGL_CONTEXT_MAJOR_VERSION 2

//dirs
#define LIBRARY_TEXTURES_FOLDER "\\Library\\Textures"
#define LIBRARY_MESHES_FOLDER "\\Library\\Meshes"
#define LIBRARY_MATERIALS_FOLDER "\\Library\\Materials"
#define LIBRARY_FOLDER "\\Library"
#define ASSETS_FOLDER "\\Assets"

//ownFormat
#define FORMAT_SIZE 3 // 3 chars
#define FORMAT_MESH {0x6d, 0x73, 0x68} //MSH
#define FORMAT_MATERIAL {0x6d, 0x61, 0x74} //MAT
#define MAX_FILE_SIZE 1073741824 / 4 //1 GB / 4 = 250 mb

#endif //_GLOBALS