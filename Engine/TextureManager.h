#ifndef _MODULE_TEXTURE_MANAGER
#define _MODULE_TEXTURE_MANAGER

#include <vector>
#include "glew\include\GL\glew.h"
#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Module.h"

#define CHECKERS_HEIGHT 256
#define CHECKERS_WIDTH 256

class Texture;

class TextureManager: public Module
{
private:
	std::vector<Texture*> textures;
	Texture* checkers;
public:
	TextureManager(const char* name, bool start_enabled = true);

	bool Init();

	UPDATE_STATUS Configuration(float dt);

	bool LoadTexture(const std::string& path, Texture& new_texture, bool hiest_quality = true);

	void LoadTextureStraightFromPath(const std::string& path);

	void AddTexture(Texture* new_texture);

	void EmptyTextures();

	Texture* GetCheckers();
};

#endif // !_MODULE_TEXTURE_MANAGER
