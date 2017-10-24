#include "Globals.h"
#include "ImportManager.h"
#include "Application.h"
#include "Texture.h"
#include "FileSystem.h"
#include "Material.h"
#include "SceneManager.h"
#include "MaterialImporter.h"

MaterialImporter::MaterialImporter()
{}

MaterialImporter::~MaterialImporter()
{}

bool MaterialImporter::Import( const aiMaterial * material, const std::string& scene_path,uint num_material, std::string& name)
{
	bool ret = true;
	size_t start = scene_path.find_last_of("\\");
	size_t count = scene_path.find_last_of(".") - start;

	std::string scene_name(scene_path.substr(start + 1, count - 1));
	std::string dir(scene_path.substr(NULL, start + 1));

	if (material->GetTextureCount(aiTextureType_NONE))
	{
		LOG("Found invalid textures");
		ret = false;
	}

	GLuint num_difusse_textures = material->GetTextureCount(aiTextureType_DIFFUSE);

	//TODO load the rest of texture types
	/*GLuint num_specular_textures = material->GetTextureCount(aiTextureType_SPECULAR);
	GLuint num_ambient_textures = material->GetTextureCount(aiTextureType_AMBIENT);
	GLuint num_emissive_textures = material->GetTextureCount(aiTextureType_EMISSIVE);
	GLuint num_height_textures = material->GetTextureCount(aiTextureType_HEIGHT);
	GLuint num_normals_textures = material->GetTextureCount(aiTextureType_NORMALS);
	GLuint num_shininess_textures = material->GetTextureCount(aiTextureType_SHININESS);
	GLuint num_opacity_textures = material->GetTextureCount(aiTextureType_OPACITY);
	GLuint num_displacement_textures = material->GetTextureCount(aiTextureType_DISPLACEMENT);
	GLuint num_lightmap_textures = material->GetTextureCount(aiTextureType_LIGHTMAP);
	GLuint num_reflection_textures = material->GetTextureCount(aiTextureType_REFLECTION);
	GLuint num_unknown_textures = material->GetTextureCount(aiTextureType_UNKNOWN);*/

	char format[FORMAT_SIZE] = FORMAT_MATERIAL;
	char* buffer = new char[MAX_FILE_SIZE];
	char* iterator = buffer;

	//First specify format
	memcpy(iterator, format, FORMAT_SIZE);
	iterator += FORMAT_SIZE;

	if (num_difusse_textures > 0)
	{
		for (int i = 0; i < num_difusse_textures; i++)
		{
			aiString Path;

			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
			{
				std::string full_path = dir + Path.data;
				start = full_path.find_last_of("\\");
				count = full_path.find_last_of(".") - start;

				std::string texture_name (full_path.substr(start + 1, count - 1)); //-1 to avoid dot

				uint text_name_length = texture_name.length();
				memcpy(iterator, &text_name_length, sizeof(uint));
				iterator += sizeof(uint);
				memcpy(iterator, texture_name.c_str(), text_name_length);
				iterator += text_name_length;
				memcpy(iterator, "\0", 1);
				iterator++;				
				
				if (!App->import_manager->ImportFromPath(full_path, IT_TEXTURE))
					LOG("Error importing texture '%s'\n", full_path.c_str());

			}
		}
	}
	else
		LOG("No difusse textures in this material");

	uint length = iterator - buffer;

	//TODO Blend and strenght functions when lightning is done

	char material_name[255];
	sprintf(material_name, "Material_%s_%i", scene_name.c_str(), num_material);
	name = material_name;

	ret = App->file_system->SaveFile(buffer, length, LIBRARY_MATERIALS_FOLDER, name.c_str(), "mm");

	return ret;
}

bool MaterialImporter::Load(const std::string & name, Material & new_material)
{
	bool ret = true;
	char* buffer = nullptr;
	char* iterator = nullptr;
	uint length = 0;

	std::string path(App->file_system->GetMaterials());
	path += "\\";
	path += name;
	path += ".mm";
	length = App->file_system->LoadFileBinary(path, &buffer);
	
	if (buffer != nullptr && length != 0)
	{
		iterator = buffer;
		iterator += FORMAT_SIZE;

		while (iterator - buffer < length)
		{
			uint texture_name_size = *iterator;
			iterator += sizeof(uint);
			std::string texture_name(iterator);
			iterator += texture_name_size + 1; // 1 for the and of char* "\0"
			Texture* texture = App->import_manager->LoadTexture(texture_name);
			new_material.AddTexture(texture);
		}
	}
	else
		ret = false;

	return ret;
}
