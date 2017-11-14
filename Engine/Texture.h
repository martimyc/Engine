#ifndef TEXTURE
#define TEXTURE

#include <string>
#include "glew\include\GL\glew.h"
#include "Resource.h"

enum TEXTURE_TYPE
{
	TT_NO_TYPE = 0,
	TT_DIFFUSE,
	TT_SPECULAR,
	TT_AMBIENT,
	TT_EMISSIVE,
	TT_HEIGHT,
	TT_NORMALS,
	TT_SHININESS,
	TT_OPACITY,
	TT_DISPLACEMENT,
	TT_LIGHTMAP,
	TT_REFLECTION,
	TT_UNKNOWN,
};

struct TextureSource
{
	unsigned int height;
	unsigned int width;
	TEXTURE_TYPE texture_type;
	GLenum gl_texure_type;
	GLuint texture_id;

	TextureSource(const TEXTURE_TYPE texture_type = TT_DIFFUSE, const GLenum gl_texure_type = GL_TEXTURE_2D, const GLuint& texture_id = 0);
	~TextureSource();

	void SetDimensions(const unsigned int& width, const unsigned int& height);

	void SetTextureType(const TEXTURE_TYPE& new_texture_type);

	void SetGLTextureType(const GLenum& new_gl_texure_type);

	void SetTextureID(const GLuint& new_id);
};

class Texture: public Resource
{
private:
	TextureSource* source;

public:
	Texture(const std::string& name, const UID& uid);
	Texture(const std::string& name, TextureSource* source);
	~Texture();

	//Gets
	const unsigned int& GetHeight() const;

	const unsigned int& GetWidth() const;	

	const TEXTURE_TYPE& GetTextureType() const;

	const GLenum& GetGLTextureType() const;

	const GLuint& GetTextureID() const;

	//Sets
	void SetSource(TextureSource* new_source);

	bool Inspector();

	bool IsLoaded() const;
};
#endif // !TEXTURE
