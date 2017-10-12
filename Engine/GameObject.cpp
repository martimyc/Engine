
#include "Mesh.h"
#include "GameObject.h"

GameObject::GameObject()
{}

GameObject::~GameObject()
{}

void GameObject::Draw(DRAW_MODE mode) const
{
	if (components.size() > 0 && mode != DM_NO_DRAW)
	{
		std::vector<Component*>::const_iterator it = components.begin();
		unsigned int num_indicies = 0;

		GLint polygonMode [2];
		glGetIntegerv(GL_POLYGON_MODE, polygonMode);

		for (; it != components.end(); ++it)
		{
			(*it)->EnableDraw();
			if ((*it)->GetType() == CT_MESH)
				num_indicies = ((Mesh*)(*it))->num_indices;
		}

		if(mode == DM_WIREFRAME && *polygonMode != GL_LINE)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		if (mode == DM_NORMAL && *polygonMode != GL_FILL)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDrawElements(GL_TRIANGLES, num_indicies, GL_UNSIGNED_INT, NULL);

		for (it = components.begin(); it != components.end(); ++it)
			(*it)->DisableDraw();

		//After draw bind buffer 0
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void GameObject::AddComponent(Component * component)
{
	components.push_back(component);
}

/*
if (texture_id != 0)
glDeleteTextures(1, &texture_id);
*/