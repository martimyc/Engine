#include "UID.h"

//Resources
#include "Asset.h"
#include "Texture.h"
#include "TextureAsset.h"
#include "Material.h"
#include "MaterialAsset.h"

//Modules
#include "FileSystem.h"
#include "ImportManager.h"
#include "ResourceManager.h"
#include "Application.h"
#include "MaterialImporter.h"

unsigned int MaterialImporter::GetTotalSize(const aiMaterial * material) const
{
	unsigned int total_size = FORMAT_SIZE;

	for (int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
	{
		aiString Path;

		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			std::string full_path = App->file_system->GetAssets() + Path.data;
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

const UID MaterialImporter::Import( const aiMaterial * material, const MaterialImportConfiguration* config)
{
	bool ret = true;

	if (material->GetTextureCount(aiTextureType_NONE) != 0)
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
	char* buffer = new char[GetTotalSize(material)];
	char* iterator = buffer;

	//First specify format
	memcpy(iterator, format, FORMAT_SIZE);
	iterator += FORMAT_SIZE;

	if (num_difusse_textures > 0)
	{
		unsigned int failed_textures = 0;
		std::vector<UID> uids;

		for (int i = 0; i < num_difusse_textures; i++)
		{
			aiString Path;

			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
			{
				std::string full_path = App->file_system->GetAssets() + Path.data;
				size_t start = full_path.find_last_of("\\");
				size_t count = full_path.find_last_of(".") - start;

				//TODO
				UID texture_uid = ImportManager::ImportClient::Import(App->import_manager, full_path, RT_TEXTURE, config->texture_import_config);
				if (texture_uid.IsNull())
				{
					LOG("Error importing texture '%s', it won't be aded to material", full_path.c_str());
					failed_textures++;
				}
				else
					uids.push_back(texture_uid);
			}
		}

		size_t size = uids.size();
		memcpy(iterator, &size, sizeof(size_t));
		iterator += sizeof(size_t);

		memcpy(iterator, &uids, SIZE_OF_UID * uids.size());
		iterator += SIZE_OF_UID * uids.size();
	}
	else
		LOG("No difusse textures in this material");

	uint length = iterator - buffer;

	//TODO Blend and strenght functions when lightning is done

	UID id(buffer, length);

	if (App->file_system->SaveFile(buffer, length, LIBRARY_MATERIALS_FOLDER, id.GetAsName(), "mm") == false)
	{
		LOG("Could not save material correctlly");
		return UID();
	}

	delete [] buffer;

	return id;
}

void MaterialImporter::Load(Material* to_load, unsigned int priority, const MaterialLoadConfiguration* config)
{
	char* buffer = nullptr;
	char* iterator = nullptr;
	uint length = 0;

	std::string path(App->file_system->GetMaterials());
	path += "\\";
	path += to_load->GetUID().GetAsName();
	path += ".mm";
	length = App->file_system->LoadFileBinary(path, &buffer);
	
	if (buffer != nullptr && length != 0)
	{
		iterator = buffer;
		iterator += FORMAT_SIZE;

		MaterialSource* new_material = new MaterialSource();

		size_t size = *iterator;
		iterator += sizeof(size_t);

		new_material->textures.resize(size);
		std::vector<UID> uids;
		memcpy(&uids, iterator, size * SIZE_OF_UID);
		iterator += size * SIZE_OF_UID;

		for (std::vector<UID>::const_iterator it = uids.begin(); it != uids.end(); ++it)
		{
			new_material->textures.push_back(ResourceManager::Client::GetTexture( App->resource_manager, *it));
			new_material->num_difusse_textures++;
		}

		to_load->SetSource(new_material);
	}
}
