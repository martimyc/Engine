#ifndef _MATERIAL
#define _MATERIAL

#include <vector>
#include "glew\include\GL\glew.h"

class Texture;
class Mesh;
enum TEXTURE_TYPES;

struct TextureWithUVs
{
	int uv_channel = 0;
	Texture* texture = nullptr;

	TextureWithUVs(Texture* texture, const GLuint& uv_channel): texture(texture), uv_channel(uv_channel)
	{}

	~TextureWithUVs()  //textures will be deleated in textures
	{}
};

class Material
{
private:
	std::string name;
	std::vector<TextureWithUVs*> textures;
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

	const int NumTextures() const;
	unsigned int GetPriority() const;
	const std::string& GetName() const;

	void AddTexture(Texture* new_text, const GLuint& uv_channel = 0);
	void Empty();
	const GLuint GetTextureCoordinateChannel(GLuint num_texture);

	void EnableDraw() const;
	void DisableDraw() const;

	void InGameObjectConfig(const GLuint& num_uv_channels);
	void LoneConfig();
};

#endif // !_MATERIAL

