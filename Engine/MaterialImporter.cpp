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

	total_size += sizeof(size_t);
	total_size += material->GetTextureCount(aiTextureType_DIFFUSE) * SIZE_OF_UID;

	total_size += sizeof(size_t);
	total_size += material->GetTextureCount(aiTextureType_SPECULAR) * SIZE_OF_UID;

	total_size += sizeof(size_t);
	total_size += material->GetTextureCount(aiTextureType_AMBIENT) * SIZE_OF_UID;

	total_size += sizeof(size_t);
	total_size += material->GetTextureCount(aiTextureType_EMISSIVE) * SIZE_OF_UID;

	total_size += sizeof(size_t);
	total_size += material->GetTextureCount(aiTextureType_HEIGHT) * SIZE_OF_UID;

	total_size += sizeof(size_t);
	total_size += material->GetTextureCount(aiTextureType_NORMALS) * SIZE_OF_UID;

	total_size += sizeof(size_t);
	total_size += material->GetTextureCount(aiTextureType_SHININESS) * SIZE_OF_UID;

	total_size += sizeof(size_t);
	total_size += material->GetTextureCount(aiTextureType_OPACITY) * SIZE_OF_UID;

	total_size += sizeof(size_t);
	total_size += material->GetTextureCount(aiTextureType_DISPLACEMENT) * SIZE_OF_UID;

	total_size += sizeof(size_t);
	total_size += material->GetTextureCount(aiTextureType_LIGHTMAP) * SIZE_OF_UID;

	total_size += sizeof(size_t);
	total_size += material->GetTextureCount(aiTextureType_REFLECTION) * SIZE_OF_UID;

	return total_size;
}

void MaterialImporter::ImportDiffuse(const std::string& scene_dir, const aiMaterial * material, char** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const
{
	std::vector<UID> uids;

	for (int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
	{
		aiString path;

		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			std::string full_path(scene_dir);

			if (all_textures_in_same_dir)
			{
				std::string relative_path(path.C_Str());
				size_t count = relative_path.find_last_of("\\");
				if(count < relative_path.size())
				relative_path = relative_path.substr(count + 1);
				full_path += relative_path;
			}
			else
				full_path += path.C_Str();
			
			UID texture_uid = ImportManager::ImportClient::Import(App->import_manager, full_path, RT_TEXTURE, import_config, load_config);

			if (texture_uid.IsNull())
				LOG("Error importing texture '%s', it won't be aded to material", full_path.c_str());
			else
				uids.push_back(texture_uid);
		}
	}

	size_t size = uids.size();
	memcpy(*iterator, &size, sizeof(size_t));
	*iterator += sizeof(size_t);

	if (uids.size() != 0)
	{
		memcpy(*iterator, &uids[0], SIZE_OF_UID * uids.size());
		*iterator += SIZE_OF_UID * uids.size();
	}
}

void MaterialImporter::ImportSpecular(const std::string& scene_dir, const aiMaterial * material, char ** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const
{
	std::vector<UID> uids;

	for (int i = 0; i < material->GetTextureCount(aiTextureType_SPECULAR); i++)
	{
		aiString path;

		if (material->GetTexture(aiTextureType_SPECULAR, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			std::string full_path(scene_dir);

			if (all_textures_in_same_dir)
			{
				std::string relative_path(path.C_Str());
				size_t count = relative_path.find_last_of("\\");
				if (count < relative_path.size())
					relative_path = relative_path.substr(count + 1);
				full_path += relative_path;
			}
			else
				full_path += path.C_Str();

			UID texture_uid = ImportManager::ImportClient::Import(App->import_manager, full_path, RT_TEXTURE, import_config, load_config);

			if (texture_uid.IsNull())
				LOG("Error importing texture '%s', it won't be aded to material", full_path.c_str());
			else
				uids.push_back(texture_uid);
		}
	}

	size_t size = uids.size();
	memcpy(*iterator, &size, sizeof(size_t));
	*iterator += sizeof(size_t);

	if (uids.size() != 0)
	{
		memcpy(*iterator, &uids[0], SIZE_OF_UID * uids.size());
		*iterator += SIZE_OF_UID * uids.size();
	}
}

void MaterialImporter::ImportAmbient(const std::string& scene_dir, const aiMaterial * material, char ** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const
{
	std::vector<UID> uids;

	for (int i = 0; i < material->GetTextureCount(aiTextureType_AMBIENT); i++)
	{
		aiString path;

		if (material->GetTexture(aiTextureType_AMBIENT, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			std::string full_path(scene_dir);

			if (all_textures_in_same_dir)
			{
				std::string relative_path(path.C_Str());
				size_t count = relative_path.find_last_of("\\");
				if (count < relative_path.size())
					relative_path = relative_path.substr(count + 1);
				full_path += relative_path;
			}
			else
				full_path += path.C_Str();

			UID texture_uid = ImportManager::ImportClient::Import(App->import_manager, full_path, RT_TEXTURE, import_config, load_config);

			if (texture_uid.IsNull())
				LOG("Error importing texture '%s', it won't be aded to material", full_path.c_str());
			else
				uids.push_back(texture_uid);
		}
	}

	size_t size = uids.size();
	memcpy(*iterator, &size, sizeof(size_t));
	*iterator += sizeof(size_t);

	if (uids.size() != 0)
	{
		memcpy(*iterator, &uids[0], SIZE_OF_UID * uids.size());
		*iterator += SIZE_OF_UID * uids.size();
	}
}

void MaterialImporter::ImportEmissive(const std::string& scene_dir, const aiMaterial * material, char ** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const
{
	std::vector<UID> uids;

	for (int i = 0; i < material->GetTextureCount(aiTextureType_EMISSIVE); i++)
	{
		aiString path;

		if (material->GetTexture(aiTextureType_EMISSIVE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			std::string full_path(scene_dir);

			if (all_textures_in_same_dir)
			{
				std::string relative_path(path.C_Str());
				size_t count = relative_path.find_last_of("\\");
				if (count < relative_path.size())
					relative_path = relative_path.substr(count + 1);
				full_path += relative_path;
			}
			else
				full_path += path.C_Str();

			UID texture_uid = ImportManager::ImportClient::Import(App->import_manager, full_path, RT_TEXTURE, import_config, load_config);

			if (texture_uid.IsNull())
				LOG("Error importing texture '%s', it won't be aded to material", full_path.c_str());
			else
				uids.push_back(texture_uid);
		}
	}

	size_t size = uids.size();
	memcpy(*iterator, &size, sizeof(size_t));
	*iterator += sizeof(size_t);

	if (uids.size() != 0)
	{
		memcpy(*iterator, &uids[0], SIZE_OF_UID * uids.size());
		*iterator += SIZE_OF_UID * uids.size();
	}
}

void MaterialImporter::ImportHeight(const std::string& scene_dir, const aiMaterial * material, char ** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const
{
	std::vector<UID> uids;

	for (int i = 0; i < material->GetTextureCount(aiTextureType_HEIGHT); i++)
	{
		aiString path;

		if (material->GetTexture(aiTextureType_HEIGHT, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			std::string full_path(scene_dir);

			if (all_textures_in_same_dir)
			{
				std::string relative_path(path.C_Str());
				size_t count = relative_path.find_last_of("\\");
				if (count < relative_path.size())
					relative_path = relative_path.substr(count + 1);
				full_path += relative_path;
			}
			else
				full_path += path.C_Str();

			UID texture_uid = ImportManager::ImportClient::Import(App->import_manager, full_path, RT_TEXTURE, import_config, load_config);

			if (texture_uid.IsNull())
				LOG("Error importing texture '%s', it won't be aded to material", full_path.c_str());
			else
				uids.push_back(texture_uid);
		}
	}

	size_t size = uids.size();
	memcpy(*iterator, &size, sizeof(size_t));
	*iterator += sizeof(size_t);

	if (uids.size() != 0)
	{
		memcpy(*iterator, &uids[0], SIZE_OF_UID * uids.size());
		*iterator += SIZE_OF_UID * uids.size();
	}
}

void MaterialImporter::ImportNormals(const std::string& scene_dir, const aiMaterial * material, char ** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const
{
	std::vector<UID> uids;

	for (int i = 0; i < material->GetTextureCount(aiTextureType_NORMALS); i++)
	{
		aiString path;

		if (material->GetTexture(aiTextureType_NORMALS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			std::string full_path(scene_dir);

			if (all_textures_in_same_dir)
			{
				std::string relative_path(path.C_Str());
				size_t count = relative_path.find_last_of("\\");
				if (count < relative_path.size())
					relative_path = relative_path.substr(count + 1);
				full_path += relative_path;
			}
			else
				full_path += path.C_Str();

			UID texture_uid = ImportManager::ImportClient::Import(App->import_manager, full_path, RT_TEXTURE, import_config, load_config);

			if (texture_uid.IsNull())
				LOG("Error importing texture '%s', it won't be aded to material", full_path.c_str());
			else
				uids.push_back(texture_uid);
		}
	}

	size_t size = uids.size();
	memcpy(*iterator, &size, sizeof(size_t));
	*iterator += sizeof(size_t);

	if (uids.size() != 0)
	{
		memcpy(*iterator, &uids[0], SIZE_OF_UID * uids.size());
		*iterator += SIZE_OF_UID * uids.size();
	}
}

void MaterialImporter::ImportShininess(const std::string& scene_dir, const aiMaterial * material, char ** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const
{
	std::vector<UID> uids;

	for (int i = 0; i < material->GetTextureCount(aiTextureType_SHININESS); i++)
	{
		aiString path;

		if (material->GetTexture(aiTextureType_SHININESS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			std::string full_path(scene_dir);

			if (all_textures_in_same_dir)
			{
				std::string relative_path(path.C_Str());
				size_t count = relative_path.find_last_of("\\");
				if (count < relative_path.size())
					relative_path = relative_path.substr(count + 1);
				full_path += relative_path;
			}
			else
				full_path += path.C_Str();

			UID texture_uid = ImportManager::ImportClient::Import(App->import_manager, full_path, RT_TEXTURE, import_config, load_config);

			if (texture_uid.IsNull())
				LOG("Error importing texture '%s', it won't be aded to material", full_path.c_str());
			else
				uids.push_back(texture_uid);
		}
	}

	size_t size = uids.size();
	memcpy(*iterator, &size, sizeof(size_t));
	*iterator += sizeof(size_t);

	if (uids.size() != 0)
	{
		memcpy(*iterator, &uids[0], SIZE_OF_UID * uids.size());
		*iterator += SIZE_OF_UID * uids.size();
	}
}

void MaterialImporter::ImportOpacity(const std::string& scene_dir, const aiMaterial * material, char ** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const
{
	std::vector<UID> uids;

	for (int i = 0; i < material->GetTextureCount(aiTextureType_OPACITY); i++)
	{
		aiString path;

		if (material->GetTexture(aiTextureType_OPACITY, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			std::string full_path(scene_dir);

			if (all_textures_in_same_dir)
			{
				std::string relative_path(path.C_Str());
				size_t count = relative_path.find_last_of("\\");
				if (count < relative_path.size())
					relative_path = relative_path.substr(count + 1);
				full_path += relative_path;
			}
			else
				full_path += path.C_Str();

			UID texture_uid = ImportManager::ImportClient::Import(App->import_manager, full_path, RT_TEXTURE, import_config, load_config);

			if (texture_uid.IsNull())
				LOG("Error importing texture '%s', it won't be aded to material", full_path.c_str());
			else
				uids.push_back(texture_uid);
		}
	}

	size_t size = uids.size();
	memcpy(*iterator, &size, sizeof(size_t));
	*iterator += sizeof(size_t);

	if (uids.size() != 0)
	{
		memcpy(*iterator, &uids[0], SIZE_OF_UID * uids.size());
		*iterator += SIZE_OF_UID * uids.size();
	}
}

void MaterialImporter::ImportDisplacement(const std::string& scene_dir, const aiMaterial * material, char ** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const
{
	std::vector<UID> uids;

	for (int i = 0; i < material->GetTextureCount(aiTextureType_DISPLACEMENT); i++)
	{
		aiString path;

		if (material->GetTexture(aiTextureType_DISPLACEMENT, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			std::string full_path(scene_dir);

			if (all_textures_in_same_dir)
			{
				std::string relative_path(path.C_Str());
				size_t count = relative_path.find_last_of("\\");
				if (count < relative_path.size())
					relative_path = relative_path.substr(count + 1);
				full_path += relative_path;
			}
			else
				full_path += path.C_Str();

			UID texture_uid = ImportManager::ImportClient::Import(App->import_manager, full_path, RT_TEXTURE, import_config, load_config);

			if (texture_uid.IsNull())
				LOG("Error importing texture '%s', it won't be aded to material", full_path.c_str());
			else
				uids.push_back(texture_uid);
		}
	}

	size_t size = uids.size();
	memcpy(*iterator, &size, sizeof(size_t));
	*iterator += sizeof(size_t);

	if (uids.size() != 0)
	{
		memcpy(*iterator, &uids[0], SIZE_OF_UID * uids.size());
		*iterator += SIZE_OF_UID * uids.size();
	}
}

void MaterialImporter::ImportLightMap(const std::string& scene_dir, const aiMaterial * material, char ** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const
{
	std::vector<UID> uids;

	for (int i = 0; i < material->GetTextureCount(aiTextureType_LIGHTMAP); i++)
	{
		aiString path;

		if (material->GetTexture(aiTextureType_LIGHTMAP, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			std::string full_path(scene_dir);

			if (all_textures_in_same_dir)
			{
				std::string relative_path(path.C_Str());
				size_t count = relative_path.find_last_of("\\");
				if (count < relative_path.size())
					relative_path = relative_path.substr(count + 1);
				full_path += relative_path;
			}
			else
				full_path += path.C_Str();

			UID texture_uid = ImportManager::ImportClient::Import(App->import_manager, full_path, RT_TEXTURE, import_config, load_config);

			if (texture_uid.IsNull())
				LOG("Error importing texture '%s', it won't be aded to material", full_path.c_str());
			else
				uids.push_back(texture_uid);
		}
	}

	size_t size = uids.size();
	memcpy(*iterator, &size, sizeof(size_t));
	*iterator += sizeof(size_t);

	if (uids.size() != 0)
	{
		memcpy(*iterator, &uids[0], SIZE_OF_UID * uids.size());
		*iterator += SIZE_OF_UID * uids.size();
	}
}

void MaterialImporter::ImportReflection(const std::string& scene_dir, const aiMaterial * material, char ** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const
{
	std::vector<UID> uids;

	for (int i = 0; i < material->GetTextureCount(aiTextureType_REFLECTION); i++)
	{
		aiString path;

		if (material->GetTexture(aiTextureType_REFLECTION, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			std::string full_path(scene_dir);

			if (all_textures_in_same_dir)
			{
				std::string relative_path(path.C_Str());
				size_t count = relative_path.find_last_of("\\");
				if (count < relative_path.size())
					relative_path = relative_path.substr(count + 1);
				full_path += relative_path;
			}
			else
				full_path += path.C_Str();

			UID texture_uid = ImportManager::ImportClient::Import(App->import_manager, full_path, RT_TEXTURE, import_config, load_config);

			if (texture_uid.IsNull())
				LOG("Error importing texture '%s', it won't be aded to material", full_path.c_str());
			else
				uids.push_back(texture_uid);
		}
	}

	size_t size = uids.size();
	memcpy(*iterator, &size, sizeof(size_t));
	*iterator += sizeof(size_t);

	if (uids.size() != 0)
	{
		memcpy(*iterator, &uids[0], SIZE_OF_UID * uids.size());
		*iterator += SIZE_OF_UID * uids.size();
	}
}

void MaterialImporter::ImportUnknown(const std::string& scene_dir, const aiMaterial * material, char ** iterator, TextureImportConfiguration* import_config, TextureLoadConfiguration* load_config, bool all_textures_in_same_dir) const
{
	std::vector<UID> uids;

	for (int i = 0; i < material->GetTextureCount(aiTextureType_UNKNOWN); i++)
	{
		aiString path;

		if (material->GetTexture(aiTextureType_UNKNOWN, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			std::string full_path(scene_dir);

			if (all_textures_in_same_dir)
			{
				std::string relative_path(path.C_Str());
				size_t count = relative_path.find_last_of("\\");
				if (count < relative_path.size())
					relative_path = relative_path.substr(count + 1);
				full_path += relative_path;
			}
			else
				full_path += path.C_Str();

			UID texture_uid = ImportManager::ImportClient::Import(App->import_manager, full_path, RT_TEXTURE, import_config, load_config);

			if (texture_uid.IsNull())
				LOG("Error importing texture '%s', it won't be aded to material", full_path.c_str());
			else
				LOG("Error importrd texture '%s' will not be added to material, unknown texture type", full_path.c_str());
		}
	}
}

void MaterialImporter::LoadDiffuse(Material * to_load, char ** iterator) const
{
	size_t size = **iterator;
	*iterator += sizeof(size_t);

	to_load->ReserveVec(size, TT_DIFFUSE);

	for (int i = 0; i < size; i++)
	{
		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;
		Texture * new_texture = App->resource_manager->UseTexture(uid, to_load);
		if(new_texture != nullptr)
			to_load->AddTexture(new_texture, TT_DIFFUSE);
		else
			LOG("Could not load texture to material");
	}
}

void MaterialImporter::LoadSpecular(Material * to_load, char ** iterator) const
{
	size_t size = **iterator;
	*iterator += sizeof(size_t);

	to_load->ReserveVec(size, TT_SPECULAR);

	for (int i = 0; i < size; i++)
	{
		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;
		Texture * new_texture = App->resource_manager->UseTexture(uid, to_load);
		if (new_texture != nullptr)
			to_load->AddTexture(new_texture, TT_SPECULAR);
		else
			LOG("Could not load texture to material");
	}
}

void MaterialImporter::LoadAmbient(Material * to_load, char ** iterator) const
{
	size_t size = **iterator;
	*iterator += sizeof(size_t);

	to_load->ReserveVec(size, TT_AMBIENT);

	for (int i = 0; i < size; i++)
	{
		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;
		Texture * new_texture = App->resource_manager->UseTexture(uid, to_load);
		if (new_texture != nullptr)
			to_load->AddTexture(new_texture, TT_AMBIENT);
		else
			LOG("Could not load texture to material");
	}
}

void MaterialImporter::LoadEmissive(Material * to_load, char ** iterator) const
{
	size_t size = **iterator;
	*iterator += sizeof(size_t);

	to_load->ReserveVec(size, TT_EMISSIVE);

	for (int i = 0; i < size; i++)
	{
		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;
		Texture * new_texture = App->resource_manager->UseTexture(uid, to_load);
		if (new_texture != nullptr)
			to_load->AddTexture(new_texture, TT_EMISSIVE);
		else
			LOG("Could not load texture to material");
	}
}

void MaterialImporter::LoadHeight(Material * to_load, char ** iterator) const
{
	size_t size = **iterator;
	*iterator += sizeof(size_t);

	to_load->ReserveVec(size, TT_HEIGHT);

	for (int i = 0; i < size; i++)
	{
		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;
		Texture * new_texture = App->resource_manager->UseTexture(uid, to_load);
		if (new_texture != nullptr)
			to_load->AddTexture(new_texture, TT_HEIGHT);
		else
			LOG("Could not load texture to material");
	}
}

void MaterialImporter::LoadNormals(Material * to_load, char ** iterator) const
{
	size_t size = **iterator;
	*iterator += sizeof(size_t);

	to_load->ReserveVec(size, TT_NORMALS);

	for (int i = 0; i < size; i++)
	{
		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;
		Texture * new_texture = App->resource_manager->UseTexture(uid, to_load);
		if (new_texture != nullptr)
			to_load->AddTexture(new_texture, TT_NORMALS);
		else
			LOG("Could not load texture to material");
	}
}

void MaterialImporter::LoadShininess(Material * to_load, char ** iterator) const
{
	size_t size = **iterator;
	*iterator += sizeof(size_t);

	to_load->ReserveVec(size, TT_SHININESS);

	for (int i = 0; i < size; i++)
	{
		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;
		Texture * new_texture = App->resource_manager->UseTexture(uid, to_load);
		if (new_texture != nullptr)
			to_load->AddTexture(new_texture, TT_SHININESS);
		else
			LOG("Could not load texture to material");
	}
}

void MaterialImporter::LoadOpacity(Material * to_load, char ** iterator) const
{
	size_t size = **iterator;
	*iterator += sizeof(size_t);

	to_load->ReserveVec(size, TT_OPACITY);

	for (int i = 0; i < size; i++)
	{
		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;
		Texture * new_texture = App->resource_manager->UseTexture(uid, to_load);
		if (new_texture != nullptr)
			to_load->AddTexture(new_texture, TT_OPACITY);
		else
			LOG("Could not load texture to material");
	}
}

void MaterialImporter::LoadDisplacement(Material * to_load, char ** iterator) const
{
	size_t size = **iterator;
	*iterator += sizeof(size_t);

	to_load->ReserveVec(size, TT_DISPLACEMENT);

	for (int i = 0; i < size; i++)
	{
		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;
		Texture * new_texture = App->resource_manager->UseTexture(uid, to_load);
		if (new_texture != nullptr)
			to_load->AddTexture(new_texture, TT_DISPLACEMENT);
		else
			LOG("Could not load texture to material");
	}
}

void MaterialImporter::LoadLightMap(Material * to_load, char ** iterator) const
{
	size_t size = **iterator;
	*iterator += sizeof(size_t);

	to_load->ReserveVec(size, TT_LIGHTMAP);

	for (int i = 0; i < size; i++)
	{
		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;
		Texture * new_texture = App->resource_manager->UseTexture(uid, to_load);
		if (new_texture != nullptr)
			to_load->AddTexture(new_texture, TT_LIGHTMAP);
		else
			LOG("Could not load texture to material");
	}
}

void MaterialImporter::LoadReflection(Material * to_load, char ** iterator) const
{
	size_t size = **iterator;
	*iterator += sizeof(size_t);

	to_load->ReserveVec(size, TT_REFLECTION);

	for (int i = 0; i < size; i++)
	{
		UID uid;
		memcpy(&uid, *iterator, SIZE_OF_UID);
		*iterator += SIZE_OF_UID;
		Texture * new_texture = App->resource_manager->UseTexture(uid, to_load);
		if (new_texture != nullptr)
			to_load->AddTexture(new_texture, TT_REFLECTION);
		else
			LOG("Could not load texture to material");
	}
}

MaterialImporter::MaterialImporter()
{}

MaterialImporter::~MaterialImporter()
{}

const UID MaterialImporter::Import(const std::string& scene_dir, const aiMaterial * material, const MaterialImportConfiguration* config) const
{
	bool ret = true;

	if (material->GetTextureCount(aiTextureType_NONE) != 0)
	{
		LOG("Found invalid textures");
		ret = false;
	}

	char format[FORMAT_SIZE] = FORMAT_MATERIAL;
	unsigned int buff_size = GetTotalSize(material);
	char* buffer = new char[buff_size];
	char* iterator = buffer;

	//First specify format
	memcpy(iterator, format, FORMAT_SIZE);
	iterator += FORMAT_SIZE;

	 ImportDiffuse(scene_dir, material, &iterator, config->texture_import_config, config->texture_load_config, config->all_textures_in_same_dir);
	 ImportSpecular(scene_dir, material, &iterator, config->texture_import_config, config->texture_load_config, config->all_textures_in_same_dir);
	 ImportAmbient(scene_dir, material, &iterator, config->texture_import_config, config->texture_load_config, config->all_textures_in_same_dir);
	 ImportEmissive(scene_dir, material, &iterator, config->texture_import_config, config->texture_load_config, config->all_textures_in_same_dir);
	 ImportHeight(scene_dir, material, &iterator, config->texture_import_config, config->texture_load_config, config->all_textures_in_same_dir);
	 ImportNormals(scene_dir, material, &iterator, config->texture_import_config, config->texture_load_config, config->all_textures_in_same_dir);
	 ImportShininess(scene_dir, material, &iterator, config->texture_import_config, config->texture_load_config, config->all_textures_in_same_dir);
	 ImportOpacity(scene_dir, material, &iterator, config->texture_import_config, config->texture_load_config, config->all_textures_in_same_dir);
	 ImportDisplacement(scene_dir, material, &iterator, config->texture_import_config, config->texture_load_config, config->all_textures_in_same_dir);
	 ImportLightMap(scene_dir, material, &iterator, config->texture_import_config, config->texture_load_config, config->all_textures_in_same_dir);
	 ImportReflection(scene_dir, material, &iterator, config->texture_import_config, config->texture_load_config, config->all_textures_in_same_dir);
	 ImportUnknown(scene_dir, material, &iterator, config->texture_import_config, config->texture_load_config, config->all_textures_in_same_dir);

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

bool MaterialImporter::Load(Material* to_load, unsigned int priority, const MaterialLoadConfiguration* config) const
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

		MaterialSource* new_material = new MaterialSource(priority);
		to_load->SetSource(new_material);

		LoadDiffuse( to_load, &iterator);
		LoadSpecular( to_load, &iterator);
		LoadAmbient( to_load, &iterator);
		LoadEmissive( to_load, &iterator);
		LoadHeight( to_load, &iterator);
		LoadNormals( to_load, &iterator);
		LoadShininess( to_load, &iterator);
		LoadOpacity( to_load, &iterator);
		LoadDisplacement( to_load, &iterator);
		LoadLightMap( to_load, &iterator);
		LoadReflection( to_load, &iterator);	


		return true;
	}
	return false;
}
