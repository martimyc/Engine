#ifndef _MATERIAL
#define _MATERIAL

#include <vector>
#include "glew\include\GL\glew.h"

class Material
{
private:
	std::vector<GLuint> texture_ids;

public:
	Material();
	~Material();

	void AssignDrawPointers();

	const int NumTextures() const;
};

#endif // !_MATERIAL

