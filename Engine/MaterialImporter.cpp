#include "UID.h"

//Resources
#include "Asset.h"
#include "Texture.h"
#include "Material.h"

//Modules
#include "FileSystem.h"
#include "ImportManager.h"
#include "Application.h"
#include "MaterialImporter.h"

unsigned int MaterialImporter::GetTotalSize(const aiMaterial * material, const std::string& scene_path) const
{
	unsigned int total_size = FORMAT_SIZE;

	for (int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
	{
		aiString Path;

		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			std::string full_path = scene_path + Path.data;
			size_t start = full_path.find_last_of("\\");
			size_t count = full_path.find_last_of(".") - start;
			total_size += sizeof(uint);
			total_size += count;
		}
	}

	return total_size;
}

MaterialImporter::MaterialImporter()
{}

MaterialImporter::~MaterialImporter()
{}

const UID& MaterialImporter::Import( const aiMaterial * material, const std::string& scene_path, const std::string& name, const MaterialImportConfiguration& config)
{
	bool ret = true;

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
	char* buffer = new char[GetTotalSize(material, scene_path)];
	char* iterator = buffer;

	//First specify format
	memcpy(iterator, format, FORMAT_SIZE);
	iterator += FORMAT_SIZE;

	//memcpy(iterator, name.c_str(), name.length());
	//iterator += name.length();

	if (num_difusse_textures > 0)
	{
		for (int i = 0; i < num_difusse_textures; i++)
		{
			aiString Path;

			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
			{
				std::string full_path = scene_path + Path.data;
				size_t start = full_path.find_last_of("\\");
				size_t count = full_path.find_last_of(".") - start;

				//TODO
				/*UID texture_uid = App->import_manager->Import(full_path, IT_TEXTURE, config.texture_import_config);
				if (texture_uid.IsNull())
					LOG("Error importing texture '%s', it won't be aded to material '%s'", full_path.c_str(), name.c_str());
				else
				{
					memcpy(iterator, texture_uid.uid, SIZE_OF_UID);
					iterator += SIZE_OF_UID;
				}*/			
			}
		}
	}
	else
		LOG("No difusse textures in this material");

	uint length = iterator - buffer;

	//TODO Blend and strenght functions when lightning is done

	UID id(buffer, length);

	if (App->file_system->SaveFile(buffer, length, LIBRARY_MATERIALS_FOLDER, id.uid, "mm") == false)
	{
		LOG("Could not save file %s correctlly", name.c_str());
		return UID();
	}

	delete [] buffer;

	return id;
}

MaterialSource* MaterialImporter::Load(const UID& id, const MaterialLoadConfiguration& config)
{
	//TODO
	return nullptr;
	/*
	Material* new_material = nullptr;
	char* buffer = nullptr;
	char* iterator = nullptr;
	uint length = 0;

	std::string path(App->file_system->GetMaterials());
	path += "\\";
	path += id.uid;
	path += ".mm";
	length = App->file_system->LoadFileBinary(path, &buffer);
	
	if (buffer != nullptr && length != 0)
	{
		iterator = buffer;
		iterator += FORMAT_SIZE;

		std::string name(iterator);
		iterator += name.length();

		new_material = new Material;

		while (iterator - buffer < length)
		{
			UID texture_uid (iterator, SIZE_OF_UID);
			iterator += SIZE_OF_UID;
			Texture* texture = App->import_manager->LoadTexture(texture_uid, config.texture_load_config);
			new_material->AddTexture(texture);
		}
	}

	return new_material;*/
}
