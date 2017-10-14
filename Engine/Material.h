#ifndef _MATERIAL
#define _MATERIAL

#include <vector>
#include "glew\include\GL\glew.h"

class Texture;

class Material
{
private:
	std::vector<Texture*> textures;

public:
	Material();
	~Material();

	void AssignDrawPointers();

	const int NumTextures() const;
};

#endif // !_MATERIAL

