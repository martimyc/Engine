#ifndef _MATERIAL
#define _MATERIAL

#include <vector>
#include "glew\include\GL\glew.h"

class Texture;
enum TEXTURE_TYPES;

class Material
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

public:
	Material();
	~Material();

	void AssignDrawPointers();

	const int NumTextures() const;

	void AddTexture(Texture* new_text);
	void Empty();
	//void ChangeTextureAt();
};

#endif // !_MATERIAL

