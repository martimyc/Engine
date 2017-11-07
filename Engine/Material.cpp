#include "imgui\imgui.h"
#include "Globals.h"
#include "Material.h"
#include "Texture.h"

Material::Material(const char* name, unsigned int priority) : Resource(RT_MATERIAL, name), priority(priority)
{}

Material::Material(const std::string & name, unsigned int priority) : Resource(RT_MATERIAL, name), priority(priority)
{}

Material::~Material() //Deleting a material does not delete its textures
{
	textures.clear();
}

void Material::ChangePriority(unsigned int new_priority)
{
	priority = new_priority;
}

const int Material::GetNumTextures() const
{
	return textures.size();
}

unsigned int Material::GetPriority() const
{
	return priority;
}

void Material::AddTexture(Texture* new_text, const GLuint& uv_channel)
{
	textures.push_back(new_text);
	num_difusse_textures++;
}

void Material::Empty()
{
	textures.clear();
	num_difusse_textures = 0;
}

void Material::EnableDraw() const
{
	GLint max_texture_units = 0;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);

	for (int i = 0; i < textures.size(); i++)
	{
		if (i > max_texture_units)
		{
			LOG("Can not render more than %i textures with this hardware, remaining textures will not be rendered", max_texture_units);
			break;
		}

		//Texture units
		glActiveTexture(GL_TEXTURE0 + i);
		//Textures
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textures[i]->GetID());
	}
}

void Material::DisableDraw() const
{
	for (int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

bool Material::Inspector()
{
	bool ret = true;

	ImGui::Text("Name: %s", name.c_str());
	ImGui::Text("Difuse textures: %i", num_difusse_textures);

	std::vector<int> textures_to_remove;

	if (ImGui::TreeNode("Textures"))
	{
		for (int i = 0; i < textures.size(); i++)
		{
			ImVec2 size(50, 50);
			ImVec2 uv0(0, 1);
			ImVec2 uv1(1, 0);

			ImGui::Image((void*)textures[i]->GetID(), size, uv0, uv1);
			ImGui::SameLine();
			ImGui::Text("Name: %s\nWidth: %i\nHeight: %i", textures[i]->GetName().c_str(), textures[i]->GetWidth(), textures[i]->GetHeight());

			if (ImGui::Button("Remove"))
			{
				LOG("Removing texture from this material");
				textures_to_remove.push_back(i);
			}
		}
		ImGui::TreePop();
	}

	for (std::vector<int>::const_iterator it = textures_to_remove.begin(); it != textures_to_remove.end(); ++it)
	{
		textures.erase(textures.begin() + (*it));
		num_difusse_textures--;
	}

	if (ImGui::Button("Delete"))
	{
		LOG("Deleting material");
		ret = false;
	}

	return ret;
}

