#include "GameObject.h"

GameObject::GameObject()
{}

GameObject::~GameObject()
{}

bool GameObject::Draw(DRAW_MODE mode) const
{
	/*glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindTexture(GL_TEXTURE_2D, img_id);

	glBindBuffer(GL_ARRAY_BUFFER, cube_uv_id);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, cube_vertex_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_indices_id);
	glIndexPointer(GL_SHORT, 0, NULL);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);*/
	return true;
}

/*
if (texture_id != 0)
glDeleteTextures(1, &texture_id);
*/