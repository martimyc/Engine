#ifndef _MATERIAL
#define _MATERIAL

#include <vector>
#include "glew\include\GL\glew.h"
#include "Asset.h"

class Texture;
enum TEXTURE_TYPES;

class Material : public Asset
{
private:
	std::vector<Texture*> textures;
	GLuint num_difusse_textures = 0;

	//TODO load all other texture types
	/*GLuint num_specular_textures = 0;
	GLuint num_ambient_textures = 0;
	GLuint num_emissive_textures = 0;
	GLuint num_height_textures = 0;
	GLuint num_normals_textures = 0;
	GLuint num_shininess_textures = 0;
	GLuint num_opacity_textures = 0;
	GLuint num_displacement_textures = 0;
	GLuint num_lightmap_textures = 0;
	GLuint num_reflection_textures = 0;
	GLuint num_unknown_textures = 0;*/

	bool edit_name = false;
	unsigned int priority;

public:
	Material(const char* name, unsigned int priority);
	~Material();

	const int GetNumTextures() const;
	unsigned int GetPriority() const;

	void AddTexture(Texture* new_text, const GLuint& uv_channel = 0);
	void Empty();

	void EnableDraw() const;
	void DisableDraw() const;

	bool Inspector();
};

#endif // !_MATERIAL

