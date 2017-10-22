#include "imgui\imgui.h"
#include "Globals.h"
#include "Material.h"
#include "Texture.h"

Material::Material(const char* name, unsigned int priority): name(name), priority(priority)
{}

Material::~Material() //Deleting a material does not delete its textures
{
	textures.clear();
}

const int Material::NumTextures() const
{
	return textures.size();
}

unsigned int Material::GetPriority() const
{
	return priority;
}

void Material::AddTexture(Texture* new_text, const GLuint& uv_channel)
{
	textures.push_back(new TextureWithUVs(new_text, uv_channel));
	num_difusse_textures++;
}

void Material::Empty()
{
	textures.clear();
	num_difusse_textures = 0;
}

const GLuint Material::GetTextureCoordinateChannel(GLuint num_texture)
{
	return textures[num_texture]->uv_channel;
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
		glBindTexture(GL_TEXTURE_2D, textures[i]->texture->id);
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

void Material::InGameObjectConfig(const GLuint& num_uv_channels)
{
	if (ImGui::TreeNode("Material"))
	{
		ImGui::Text("Difuse textures: %i", num_difusse_textures);

		std::vector<int> textures_to_remove;

		for (int i = 0; i < textures.size(); i++)
		{
			char text_name[255];
			sprintf(text_name, "Texture: %i", i);

			if (ImGui::TreeNode(text_name))
			{
				ImGui::Text(textures[i]->texture->path.c_str());
				if (ImGui::InputInt("UV channel:", &textures[i]->uv_channel))
				{
					if (textures[i]->uv_channel < 0)
						textures[i]->uv_channel = 0;
					else if (textures[i]->uv_channel >= num_uv_channels)
					{
						LOG("No more UV channels");
						textures[i]->uv_channel = num_uv_channels - 1;
					}
				}

				if (ImGui::Button("Delete"))
				{
					LOG("Removing texture from this material");
					textures_to_remove.push_back(i);
				}

				ImGui::TreePop();
			}
		}
		for (std::vector<int>::const_iterator it = textures_to_remove.begin(); it != textures_to_remove.end(); ++it)
		{
			textures.erase(textures.begin() + (*it));
			num_difusse_textures--;
		}

		ImGui::TreePop();
	}
}

void Material::LoneConfig()
{
	ImGui::Text("Difuse textures: %i", num_difusse_textures);

	std::vector<int> textures_to_remove;

	for (int i = 0; i < textures.size(); i++)
	{
		char text_name[255];
		sprintf(text_name, "Texture: %i", i);

		if (ImGui::TreeNode(text_name))
		{
			ImGui::Text(textures[i]->texture->path.c_str());

			if (ImGui::Button("Delete"))
			{
				LOG("Removing texture from this material");
				textures_to_remove.push_back(i);
			}

			ImGui::TreePop();
		}
	}
	for (std::vector<int>::const_iterator it = textures_to_remove.begin(); it != textures_to_remove.end(); ++it)
	{
		textures.erase(textures.begin() + (*it));
		num_difusse_textures--;
	}
}

