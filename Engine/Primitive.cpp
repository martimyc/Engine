#include "Primitive.h"
#include "glew\include\GL\glew.h"
#include "SDL2\include\SDL_opengl.h"

Primitive::Primitive(PRIMITIVE_TYPE type) :type(type)
{

}

Primitive::Primitive(const Primitive & copy) : type(copy.type)
{

}

Primitive::~Primitive()
{

}

void Primitive::Initialize()
{
	//Save vertex
	glGenBuffers(1, (GLuint*)&(id_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STATIC_DRAW);

	//Save index
	glGenBuffers(1, (GLuint*)&(id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * num_indices, indices, GL_STATIC_DRAW);
}

void Primitive::Draw()
{
	if (id_indices == 0 || id_vertices == 0)
		return;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawElements(GL_TRIANGLES, sizeof(GLuint) * num_vertices / 3, GL_UNSIGNED_INT, NULL);
}