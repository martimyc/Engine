#include "imgui\imgui.h"
#include "Globals.h"
#include "Material.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "Application.h"
#include "Texture.h"

MaterialSource::MaterialSource(unsigned int priority): priority(priority)
{}

MaterialSource::~MaterialSource()
{
	diffuse_textures.clear();
	specular_textures.clear();
	ambient_textures.clear();
	emissive_textures.clear();
	height_textures.clear();
	normals_textures.clear();
	shininess_textures.clear();
	opacity_textures.clear();
	displacement_textures.clear();
	lightmap_textures.clear();
	reflection_textures.clear();
}

void MaterialSource::ChangePriority(unsigned int new_priority)
{
	priority = new_priority;
}

unsigned int MaterialSource::GetNumAllTextures() const
{
	uint num_textures = 0;
	num_textures += diffuse_textures.size();
	num_textures += specular_textures.size();
	num_textures += ambient_textures.size();
	num_textures += emissive_textures.size();
	num_textures += height_textures.size();
	num_textures += normals_textures.size();
	num_textures += shininess_textures.size();
	num_textures += opacity_textures.size();
	num_textures += displacement_textures.size();
	num_textures += lightmap_textures.size();
	num_textures += reflection_textures.size();
	return num_textures;
}

unsigned int MaterialSource::GetNumTextures(TEXTURE_TYPE type) const
{
	switch (type)
	{
	case TT_DIFFUSE: return diffuse_textures.size();
	case TT_SPECULAR: return specular_textures.size();
	case TT_AMBIENT: return ambient_textures.size();
	case TT_EMISSIVE: return emissive_textures.size();
	case TT_HEIGHT: return height_textures.size();
	case TT_NORMALS: return normals_textures.size();
	case TT_SHININESS: return shininess_textures.size();
	case TT_OPACITY: return opacity_textures.size();
	case TT_DISPLACEMENT: return displacement_textures.size();
	case TT_LIGHTMAP: return lightmap_textures.size();
	case TT_REFLECTION: return reflection_textures.size();
	}
	return 0;
}

unsigned int MaterialSource::GetPriority() const
{
	return priority;
}

void MaterialSource::AddTexture(Texture* new_text, TEXTURE_TYPE type)
{
	switch (type)
	{
	case TT_DIFFUSE: diffuse_textures.push_back(new_text); break;
	case TT_SPECULAR: specular_textures.push_back(new_text); break;
	case TT_AMBIENT: ambient_textures.push_back(new_text); break;
	case TT_EMISSIVE: emissive_textures.push_back(new_text); break;
	case TT_HEIGHT: height_textures.push_back(new_text); break;
	case TT_NORMALS: normals_textures.push_back(new_text); break;
	case TT_SHININESS: shininess_textures.push_back(new_text); break;
	case TT_OPACITY: opacity_textures.push_back(new_text); break;
	case TT_DISPLACEMENT: displacement_textures.push_back(new_text); break;
	case TT_LIGHTMAP: lightmap_textures.push_back(new_text); break;
	case TT_REFLECTION: reflection_textures.push_back(new_text); break;
	}
}

void MaterialSource::ReserveVec(unsigned int size, TEXTURE_TYPE type)
{
	switch (type)
	{
	case TT_DIFFUSE: diffuse_textures.reserve(size); break;
	case TT_SPECULAR: specular_textures.reserve(size); break;
	case TT_AMBIENT: ambient_textures.reserve(size); break;
	case TT_EMISSIVE: emissive_textures.reserve(size); break;
	case TT_HEIGHT: height_textures.reserve(size); break;
	case TT_NORMALS: normals_textures.reserve(size); break;
	case TT_SHININESS: shininess_textures.reserve(size); break;
	case TT_OPACITY: opacity_textures.reserve(size); break;
	case TT_DISPLACEMENT: displacement_textures.reserve(size); break;
	case TT_LIGHTMAP: lightmap_textures.reserve(size); break;
	case TT_REFLECTION: reflection_textures.reserve(size); break;
	}
}

void MaterialSource::EnableDraw() const
{
	GLint max_texture_units = 0;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);

	for (int i = 0; i < diffuse_textures.size(); i++)
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
		glBindTexture(GL_TEXTURE_2D, diffuse_textures[i]->GetTextureID());
	}

	//TODO enable all other texture types
}

void MaterialSource::DisableDraw() const
{
	for (int i = 0; i < diffuse_textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	//TODO disable all other texture types
}

void MaterialSource::Inspector()
{
	ImGui::Text("Difuse textures: %i", diffuse_textures.size());
	ImGui::Text("Specular textures: %i", specular_textures.size());
	ImGui::Text("Ambient textures: %i", ambient_textures.size());
	ImGui::Text("Emissive textures: %i", emissive_textures.size());
	ImGui::Text("Height textures: %i", height_textures.size());
	ImGui::Text("Normals textures: %i", normals_textures.size());
	ImGui::Text("Shininess textures: %i", shininess_textures.size());
	ImGui::Text("Opacity textures: %i", opacity_textures.size());
	ImGui::Text("Displacement textures: %i", displacement_textures.size());
	ImGui::Text("Light Map textures: %i", lightmap_textures.size());
	ImGui::Text("Reflection textures: %i", reflection_textures.size());

	 InspectorDiffuse();
	 InspectorSpecular();
	 InspectorAmbient();
	 InspectorEmissive();
	 InspectorHeight();
	 InspectorNormals();
	 InspectorShininess();
	 InspectorOpacity();
	 InspectorDisplacement();
	 InspectorLightMap();
	 InspectorReflection();
}

void MaterialSource::InspectorDiffuse()
{
	ImGuiTreeNodeFlags flags = 0;

	if (diffuse_textures.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (ImGui::TreeNodeEx("Difuse Textures", flags))
	{
		std::vector<int> textures_to_remove;
		for (int i = 0; i < diffuse_textures.size(); i++)
		{
			ImVec2 size(50, 50);
			ImVec2 uv0(0, 1);
			ImVec2 uv1(1, 0);

			ImGui::Image((void*)diffuse_textures[i]->GetTextureID(), size, uv0, uv1);
			ImGui::SameLine();
			ImGui::Text("Name: %s\nWidth: %i\nHeight: %i", diffuse_textures[i]->GetName().c_str(), diffuse_textures[i]->GetWidth(), diffuse_textures[i]->GetHeight());

			if (ImGui::Button("Remove"))
			{
				LOG("Removing texture from this material");
				textures_to_remove.push_back(i);
			}
		}

		for (std::vector<int>::const_iterator it = textures_to_remove.begin(); it != textures_to_remove.end(); ++it)
		{
			diffuse_textures.erase(diffuse_textures.begin() + (*it));
		}
		ImGui::TreePop();
	}
}

void MaterialSource::InspectorSpecular()
{
	ImGuiTreeNodeFlags flags = 0;

	if (specular_textures.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (ImGui::TreeNodeEx("Specular Textures", flags))
	{
		std::vector<int> textures_to_remove;
		for (int i = 0; i < specular_textures.size(); i++)
		{
			ImVec2 size(50, 50);
			ImVec2 uv0(0, 1);
			ImVec2 uv1(1, 0);

			ImGui::Image((void*)specular_textures[i]->GetTextureID(), size, uv0, uv1);
			ImGui::SameLine();
			ImGui::Text("Name: %s\nWidth: %i\nHeight: %i", specular_textures[i]->GetName().c_str(), specular_textures[i]->GetWidth(), specular_textures[i]->GetHeight());

			if (ImGui::Button("Remove"))
			{
				LOG("Removing texture from this material");
				textures_to_remove.push_back(i);
			}
		}

		for (std::vector<int>::const_iterator it = textures_to_remove.begin(); it != textures_to_remove.end(); ++it)
		{
			specular_textures.erase(specular_textures.begin() + (*it));
		}
		ImGui::TreePop();
	}
}

void MaterialSource::InspectorAmbient()
{
	ImGuiTreeNodeFlags flags = 0;

	if (ambient_textures.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (ImGui::TreeNodeEx("Ambient Textures", flags))
	{
		std::vector<int> textures_to_remove;
		for (int i = 0; i < ambient_textures.size(); i++)
		{
			ImVec2 size(50, 50);
			ImVec2 uv0(0, 1);
			ImVec2 uv1(1, 0);

			ImGui::Image((void*)ambient_textures[i]->GetTextureID(), size, uv0, uv1);
			ImGui::SameLine();
			ImGui::Text("Name: %s\nWidth: %i\nHeight: %i", ambient_textures[i]->GetName().c_str(), ambient_textures[i]->GetWidth(), ambient_textures[i]->GetHeight());

			if (ImGui::Button("Remove"))
			{
				LOG("Removing texture from this material");
				textures_to_remove.push_back(i);
			}
		}

		for (std::vector<int>::const_iterator it = textures_to_remove.begin(); it != textures_to_remove.end(); ++it)
		{
			ambient_textures.erase(ambient_textures.begin() + (*it));
		}
		ImGui::TreePop();
	}
}

void MaterialSource::InspectorEmissive()
{
	ImGuiTreeNodeFlags flags = 0;

	if (emissive_textures.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (ImGui::TreeNodeEx("Emissive Textures", flags))
	{
		std::vector<int> textures_to_remove;
		for (int i = 0; i < emissive_textures.size(); i++)
		{
			ImVec2 size(50, 50);
			ImVec2 uv0(0, 1);
			ImVec2 uv1(1, 0);

			ImGui::Image((void*)emissive_textures[i]->GetTextureID(), size, uv0, uv1);
			ImGui::SameLine();
			ImGui::Text("Name: %s\nWidth: %i\nHeight: %i", emissive_textures[i]->GetName().c_str(), emissive_textures[i]->GetWidth(), emissive_textures[i]->GetHeight());

			if (ImGui::Button("Remove"))
			{
				LOG("Removing texture from this material");
				textures_to_remove.push_back(i);
			}
		}

		for (std::vector<int>::const_iterator it = textures_to_remove.begin(); it != textures_to_remove.end(); ++it)
		{
			emissive_textures.erase(emissive_textures.begin() + (*it));
		}
		ImGui::TreePop();
	}
}

void MaterialSource::InspectorHeight()
{
	ImGuiTreeNodeFlags flags = 0;

	if (height_textures.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (ImGui::TreeNodeEx("Height Textures", flags))
	{
		std::vector<int> textures_to_remove;
		for (int i = 0; i < height_textures.size(); i++)
		{
			ImVec2 size(50, 50);
			ImVec2 uv0(0, 1);
			ImVec2 uv1(1, 0);

			ImGui::Image((void*)height_textures[i]->GetTextureID(), size, uv0, uv1);
			ImGui::SameLine();
			ImGui::Text("Name: %s\nWidth: %i\nHeight: %i", height_textures[i]->GetName().c_str(), height_textures[i]->GetWidth(), height_textures[i]->GetHeight());

			if (ImGui::Button("Remove"))
			{
				LOG("Removing texture from this material");
				textures_to_remove.push_back(i);
			}
		}

		for (std::vector<int>::const_iterator it = textures_to_remove.begin(); it != textures_to_remove.end(); ++it)
		{
			height_textures.erase(height_textures.begin() + (*it));
		}
		ImGui::TreePop();
	}
}

void MaterialSource::InspectorNormals()
{
	ImGuiTreeNodeFlags flags = 0;

	if (normals_textures.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (ImGui::TreeNodeEx("Normals Textures", flags))
	{
		std::vector<int> textures_to_remove;
		for (int i = 0; i < normals_textures.size(); i++)
		{
			ImVec2 size(50, 50);
			ImVec2 uv0(0, 1);
			ImVec2 uv1(1, 0);

			ImGui::Image((void*)normals_textures[i]->GetTextureID(), size, uv0, uv1);
			ImGui::SameLine();
			ImGui::Text("Name: %s\nWidth: %i\nHeight: %i", normals_textures[i]->GetName().c_str(), normals_textures[i]->GetWidth(), normals_textures[i]->GetHeight());

			if (ImGui::Button("Remove"))
			{
				LOG("Removing texture from this material");
				textures_to_remove.push_back(i);
			}
		}

		for (std::vector<int>::const_iterator it = textures_to_remove.begin(); it != textures_to_remove.end(); ++it)
		{
			normals_textures.erase(normals_textures.begin() + (*it));
		}
		ImGui::TreePop();
	}
}

void MaterialSource::InspectorShininess()
{
	ImGuiTreeNodeFlags flags = 0;

	if (shininess_textures.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (ImGui::TreeNodeEx("Shinines Textures", flags))
	{
		std::vector<int> textures_to_remove;
		for (int i = 0; i < shininess_textures.size(); i++)
		{
			ImVec2 size(50, 50);
			ImVec2 uv0(0, 1);
			ImVec2 uv1(1, 0);

			ImGui::Image((void*)shininess_textures[i]->GetTextureID(), size, uv0, uv1);
			ImGui::SameLine();
			ImGui::Text("Name: %s\nWidth: %i\nHeight: %i", shininess_textures[i]->GetName().c_str(), shininess_textures[i]->GetWidth(), shininess_textures[i]->GetHeight());

			if (ImGui::Button("Remove"))
			{
				LOG("Removing texture from this material");
				textures_to_remove.push_back(i);
			}
		}

		for (std::vector<int>::const_iterator it = textures_to_remove.begin(); it != textures_to_remove.end(); ++it)
		{
			shininess_textures.erase(shininess_textures.begin() + (*it));
		}
		ImGui::TreePop();
	}
}

void MaterialSource::InspectorOpacity()
{
	ImGuiTreeNodeFlags flags = 0;

	if (opacity_textures.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (ImGui::TreeNodeEx("Opacity Textures", flags))
	{
		std::vector<int> textures_to_remove;
		for (int i = 0; i < opacity_textures.size(); i++)
		{
			ImVec2 size(50, 50);
			ImVec2 uv0(0, 1);
			ImVec2 uv1(1, 0);

			ImGui::Image((void*)opacity_textures[i]->GetTextureID(), size, uv0, uv1);
			ImGui::SameLine();
			ImGui::Text("Name: %s\nWidth: %i\nHeight: %i", opacity_textures[i]->GetName().c_str(), opacity_textures[i]->GetWidth(), opacity_textures[i]->GetHeight());

			if (ImGui::Button("Remove"))
			{
				LOG("Removing texture from this material");
				textures_to_remove.push_back(i);
			}
		}

		for (std::vector<int>::const_iterator it = textures_to_remove.begin(); it != textures_to_remove.end(); ++it)
		{
			opacity_textures.erase(opacity_textures.begin() + (*it));
		}
		ImGui::TreePop();
	}
}

void MaterialSource::InspectorDisplacement()
{
	ImGuiTreeNodeFlags flags = 0;

	if (displacement_textures.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (ImGui::TreeNodeEx("Displacement Textures", flags))
	{
		std::vector<int> textures_to_remove;
		for (int i = 0; i < displacement_textures.size(); i++)
		{
			ImVec2 size(50, 50);
			ImVec2 uv0(0, 1);
			ImVec2 uv1(1, 0);

			ImGui::Image((void*)displacement_textures[i]->GetTextureID(), size, uv0, uv1);
			ImGui::SameLine();
			ImGui::Text("Name: %s\nWidth: %i\nHeight: %i", displacement_textures[i]->GetName().c_str(), displacement_textures[i]->GetWidth(), displacement_textures[i]->GetHeight());

			if (ImGui::Button("Remove"))
			{
				LOG("Removing texture from this material");
				textures_to_remove.push_back(i);
			}
		}

		for (std::vector<int>::const_iterator it = textures_to_remove.begin(); it != textures_to_remove.end(); ++it)
		{
			displacement_textures.erase(displacement_textures.begin() + (*it));
		}
		ImGui::TreePop();
	}
}

void MaterialSource::InspectorLightMap()
{
	ImGuiTreeNodeFlags flags = 0;

	if (lightmap_textures.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (ImGui::TreeNodeEx("Light Map Textures", flags))
	{
		std::vector<int> textures_to_remove;
		for (int i = 0; i < lightmap_textures.size(); i++)
		{
			ImVec2 size(50, 50);
			ImVec2 uv0(0, 1);
			ImVec2 uv1(1, 0);

			ImGui::Image((void*)lightmap_textures[i]->GetTextureID(), size, uv0, uv1);
			ImGui::SameLine();
			ImGui::Text("Name: %s\nWidth: %i\nHeight: %i", lightmap_textures[i]->GetName().c_str(), lightmap_textures[i]->GetWidth(), lightmap_textures[i]->GetHeight());

			if (ImGui::Button("Remove"))
			{
				LOG("Removing texture from this material");
				textures_to_remove.push_back(i);
			}
		}

		for (std::vector<int>::const_iterator it = textures_to_remove.begin(); it != textures_to_remove.end(); ++it)
		{
			lightmap_textures.erase(lightmap_textures.begin() + (*it));
		}
		ImGui::TreePop();
	}
}

void MaterialSource::InspectorReflection()
{
	ImGuiTreeNodeFlags flags = 0;

	if (reflection_textures.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (ImGui::TreeNodeEx("Reflection Textures", flags))
	{
		std::vector<int> textures_to_remove;
		for (int i = 0; i < reflection_textures.size(); i++)
		{
			ImVec2 size(50, 50);
			ImVec2 uv0(0, 1);
			ImVec2 uv1(1, 0);

			ImGui::Image((void*)reflection_textures[i]->GetTextureID(), size, uv0, uv1);
			ImGui::SameLine();
			ImGui::Text("Name: %s\nWidth: %i\nHeight: %i", reflection_textures[i]->GetName().c_str(), reflection_textures[i]->GetWidth(), reflection_textures[i]->GetHeight());

			if (ImGui::Button("Remove"))
			{
				LOG("Removing texture from this material");
				textures_to_remove.push_back(i);
			}
		}

		for (std::vector<int>::const_iterator it = textures_to_remove.begin(); it != textures_to_remove.end(); ++it)
		{
			reflection_textures.erase(reflection_textures.begin() + (*it));
		}
		ImGui::TreePop();
	}
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

unsigned int Material::GetNumAllTextures() const
{
	if (source != nullptr)
		return source->GetNumAllTextures();
	LOG("Trying to acces non loaded material");
	return 0;
}

unsigned int Material::GetNumTextures(TEXTURE_TYPE type) const
{
	if (source != nullptr)
		return source->GetNumTextures(type);
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

void Material::AddTexture(Texture * new_text, TEXTURE_TYPE type)
{
	if (source != nullptr)
		source->AddTexture(new_text, type);
	else
		LOG("Trying to acces non loaded material");
}

void Material::ReserveVec(unsigned int size, TEXTURE_TYPE type)
{
	if (source != nullptr)
		source->ReserveVec(size, type);
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

void Material::SetSource(MaterialSource * source)
{
	this->source = source;
}
