#ifndef _MODULE_TEXTURES
#define _MODULE_TEXTURES

#include <list>
#include "glew\include\GL\glew.h"
#include "DevIL1.8.0\DevIL Windows SDK\include\IL\il.h"
#include "DevIL1.8.0\DevIL Windows SDK\include\IL\ilu.h"
#include "DevIL1.8.0\DevIL Windows SDK\include\IL\ilut.h"
#include "Module.h"

class Textures: public Module
{
private:
	std::list <GLuint> textures;
public:
	Textures(const char* name, bool start_enabled = true);

	bool Init();

	GLuint LoadTexture(const char* path);
};

#endif // !_MODULE_TEXTURES
