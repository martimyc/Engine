
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
		std::vector<Component*> meshes;
		Component* material = nullptr;

		GLint polygonMode[2];
		glGetIntegerv(GL_POLYGON_MODE, polygonMode);

		for (; it != components.end(); ++it)
		{
			if ((*it)->Enabled())
			{
				if ((*it)->GetType() == CT_MESH)
					meshes.push_back(*it);
				if ((*it)->GetType() == CT_MATERIAL)
					material = *it;
			}		
		}

		if (mode == DM_WIREFRAME && *polygonMode != GL_LINE)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		if (mode == DM_NORMAL && *polygonMode != GL_FILL)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if(meshes.size() > 0)
			glEnableClientState(GL_VERTEX_ARRAY);

		if (material != nullptr && meshes.size() > 0)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			material->AssignDrawPointers();
		}

		for (std::vector<Component*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it)
		{
			//Get num indicies
			num_indicies = ((Mesh*)(*it))->num_indices;

			//Assign all draw pointers
			(*it)->AssignDrawPointers();

			glDrawElements(GL_TRIANGLES, num_indicies, GL_UNSIGNED_INT, NULL);
		}

		//Disable texture 2D client state & unbind text buffer
		if (material != nullptr && meshes.size() > 0)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		//Disable vertex array states
		if (meshes.size() > 0)
		{
			//After draw bind buffer 0
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//Indicies buffer unbind
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glDisableClientState(GL_VERTEX_ARRAY);
		}
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