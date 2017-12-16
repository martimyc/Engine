#ifndef TEXTURE
#define TEXTURE

#include <string>
#include "glew\include\GL\glew.h"
#include "Resource.h"

struct TextureSource
{
	unsigned int height;
	unsigned int width;
	GLenum gl_texure_type;
	GLuint texture_id;

	TextureSource(const GLenum gl_texure_type = GL_TEXTURE_2D, const GLuint& texture_id = 0);
	~TextureSource();

	void SetDimensions(const unsigned int& width, const unsigned int& height);

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

	const GLenum& GetGLTextureType() const;

	const GLuint& GetTextureID() const;

	//Sets
	bool Inspector();

	bool IsLoaded() const;

	void UnLoad();

	void SetSource(TextureSource* source);
};
#endif // !TEXTURE
