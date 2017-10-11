
#include "Mesh.h"
#include "GameObject.h"

GameObject::GameObject()
{}

GameObject::GameObject(const Mesh * mesh): mesh(mesh)
{}

GameObject::~GameObject()
{}

void GameObject::Draw(DRAW_MODE mode, GLuint text) const
{
	mesh->Draw(text);
}

/*
if (texture_id != 0)
glDeleteTextures(1, &texture_id);
*/