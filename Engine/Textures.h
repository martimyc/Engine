#ifndef _MODULE_TEXTURES
#define _MODULE_TEXTURES

#include <list>
#include "glew\include\GL\glew.h"
#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Module.h"

#define CHECKERS_HEIGHT 256
#define CHECKERS_WIDTH 256

class Texture;

class Textures: public Module
{
private:
	//list better this just now
	std::vector <Texture*> textures;
public:
	Textures(const char* name, bool start_enabled = true);

	bool Init();

	bool LoadTexture(const std::string& path, Texture& new_texture, bool hiest_quality = true);

	void LoadTextureStraightFromPath(const std::string& path);

	void AddTexture(Texture* new_texture);

	Texture* GetCheckers();
};

#endif // !_MODULE_TEXTURES
