#include "imgui\imgui.h"
#include "Globals.h"
#include "Material.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "Application.h"
#include "Texture.h"

MaterialSource::MaterialSource()
{}

MaterialSource::~MaterialSource()
{
	textures.clear();
}

void MaterialSource::ChangePriority(unsigned int new_priority)
{
	priority = new_priority;
}

const int MaterialSource::GetNumTextures() const
{
	return textures.size();
}

unsigned int MaterialSource::GetPriority() const
{
	return priority;
}

void MaterialSource::AddTexture(Texture* new_text, const GLuint& uv_channel)
{
	textures.push_back(new_text);
	num_difusse_textures++;
}

void MaterialSource::Empty()
{
	textures.clear();
	num_difusse_textures = 0;
}

void MaterialSource::EnableDraw() const
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
		glBindTexture(GL_TEXTURE_2D, textures[i]->GetTextureID());
	}
}

void MaterialSource::DisableDraw() const
{
	for (int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

void MaterialSource::Inspector()
{
	ImGui::Text("Difuse textures: %i", num_difusse_textures);

	std::vector<int> textures_to_remove;

	if (ImGui::TreeNode("Textures"))
	{
		for (int i = 0; i < textures.size(); i++)
		{
			ImVec2 size(50, 50);
			ImVec2 uv0(0, 1);
			ImVec2 uv1(1, 0);

			ImGui::Image((void*)textures[i]->GetTextureID(), size, uv0, uv1);
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
}

void MaterialSource::Use(const GameObject* go)
{
	for (std::vector<Texture*>::iterator it = textures.begin(); it != textures.end(); ++it)
		App->resource_manager->Use((*it)->GetUID(), go);
}

//Material
Material::Material(const std::string name, const UID& uid) : Resource(RT_MATERIAL, name, uid), source(nullptr)
{}

Material::Material(const std::string name, MaterialSource * source) : Resource(RT_MATERIAL, name), source(source)
{}

Material::~Material() //Deleting a material does not delete its textures
{
	if (source != nullptr)
		delete source;
}

void Material::ChangePriority(unsigned int new_priority)
{
	if (source != nullptr)
		source->ChangePriority(new_priority);
	else
		LOG("Trying to acces non loaded material");
}

const int Material::GetNumTextures() const
{
	if (source != nullptr)
		return source->GetNumTextures();
	LOG("Trying to acces non loaded material");
	return 0;
}

unsigned int Material::GetPriority() const
{
	if (source != nullptr)
		return source->GetPriority();
	LOG("Trying to acces non loaded material");
	return 0;
}

void Material::AddTexture(Texture * new_text, const GLuint & uv_channel)
{
	if (source != nullptr)
		source->AddTexture(new_text, uv_channel);
	else
		LOG("Trying to acces non loaded material");
}

void Material::Empty()
{
	if (source != nullptr)
		source->Empty();
	else
		LOG("Trying to acces non loaded material");
}

void Material::EnableDraw() const
{
	if (source != nullptr)
		source->EnableDraw();
	else
		LOG("Trying to acces non loaded material");
}

void Material::DisableDraw() const
{
	if (source != nullptr)
		source->DisableDraw();
	else
		LOG("Trying to acces non loaded material");
}

bool Material::Inspector()
{
	bool ret = true;

	ImGui::Text("Name: %s", name.c_str());

	source->Inspector();

	if (ImGui::Button("Delete"))
	{
		LOG("Deleting material");
		ret = false;
	}

	return ret;
}

bool Material::IsLoaded() const
{
	return source != nullptr;
}

void Material::Use(const GameObject* go)
{
	if (source != nullptr)
		source->Use(go);
	else
		LOG("Trying to acces non loaded material");
}

void Material::SetSource(MaterialSource * source)
{
	this->source = source;
}
