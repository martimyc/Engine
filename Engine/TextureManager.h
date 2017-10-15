#ifndef _MODULE_TEXTURE_MANAGER
#define _MODULE_TEXTURE_MANAGER

#include <vector>
#include "glew\include\GL\glew.h"
#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"
#include "Module.h"

#define CHECKERS_HEIGHT 256
#define CHECKERS_WIDTH 256

class Texture;

class TextureManager: public Module
{
private:
	std::vector<Texture*> textures;
	Texture* checkers;
	int texture_to_draw = 0;
	bool debug_textures = false;
public:
	TextureManager(const char* name, bool start_enabled = true);

	bool Init();

	UPDATE_STATUS Configuration(float dt);
	void DrawTexture( unsigned int num_texture);

	bool LoadTexture(const std::string& path, Texture& new_texture, bool hiest_quality = true);

	void LoadTextureStraightFromPath(const std::string& path);

	void AddTexture(Texture* new_texture);

	void EmptyTextures();

	Texture* GetCheckers();
	Texture* GetSecondTexture();
	const int GetTextureToDraw() const;
	bool DebugTextures() const;
};

#endif // !_MODULE_TEXTURE_MANAGER
