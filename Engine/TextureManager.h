#ifndef _MODULE_TEXTURE_MANAGER
#define _MODULE_TEXTURE_MANAGER

#include <vector>
#include <string>
#include "glew\include\GL\glew.h"
#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"
#include "Module.h"

#define CHECKERS_HEIGHT 256
#define CHECKERS_WIDTH 256

class Texture;
enum TEXTURE_TYPES;

class TextureManager: public Module
{
private:
	std::vector<Texture*> textures;
	Texture* checkers;
	int texture_to_draw = 0;
	bool debug_textures = false;
	int current_material = 0;

	bool LoadTexture(const std::string& path, Texture& new_texture);
	Texture* CreateTexture(const std::string& path, const TEXTURE_TYPES type, const GLenum dimensions = GL_TEXTURE_2D, const GLuint& id = 0);

public:
	TextureManager(const char* name, bool start_enabled = true);
	~TextureManager();

	bool Init();

	UPDATE_STATUS Configuration(float dt);
	void DrawTexture( unsigned int num_texture);

	Texture* LoadTextureStraightFromPath(const std::string& path);

	void AddTexture(Texture* new_texture);

	void EmptyTextures();

	Texture* GetCheckers();
	Texture* GetSecondTexture();
	const int GetTextureToDraw() const;
	bool DebugTextures() const;

	bool Exsists(const std::string& path) const;
};

#endif // !_MODULE_TEXTURE_MANAGER
