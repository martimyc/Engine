#include "GameObject.h"

GameObject::GameObject()
{
}

GameObject::GameObject(const GLuint vertex_id, const GLuint indices_id, const GLuint normals_id, GLuint uv_id, const GLuint texture_id): vertex_id(vertex_id), indices_id(indices_id), normals_id(normals_id), uv_id(uv_id), texture_id(texture_id)
{
}

GameObject::~GameObject()
{
	if (vertex_id != 0)
		glDeleteBuffers(1, &vertex_id);
	if (indices_id != 0)
		glDeleteBuffers(1, &indices_id);
	if (normals_id != 0)
		glDeleteBuffers(1, &normals_id);
	if (uv_id != 0)
		glDeleteBuffers(1, &uv_id);
	if (texture_id != 0)
		glDeleteTextures(1, &texture_id);
}
