#include <windows.h>
#include <stdio.h>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include "Dirent\dirent.h"
#include "AssetDirectory.h"
#include "ImportManager.h"
#include "ResourceManager.h"
#include "Application.h"
#include "FileSystem.h"

#ifdef WIN32
#define stat _stat
#endif

FileSystem::FileSystem(const char * name, bool start_enabled): Module(name, start_enabled)
{}

FileSystem::~FileSystem()
{}

bool FileSystem::Init()
{
	char my_path[MAX_PATH + 1];
	GetCurrentDirectory(MAX_PATH, my_path);
	working_directory = my_path;

	std::string assets(working_directory);
	assets += "\\Assets";

	if (!Exsists(assets))
		if (!CreateFolder("Assets"))
			LOG("Assets folder could not be created");

	std::string library = working_directory;
	library += LIBRARY_FOLDER;

	if (!Exsists(library))
		if (!CreateFolder("Library", true))
			LOG("Library folder could not be created");

	std::string meshes = working_directory;
	meshes += LIBRARY_MESHES_FOLDER;

	if (!Exsists(meshes))
		if (!CreateFolder("Meshes", false, LIBRARY_FOLDER))
			LOG("Meshes folder could not be created");

	std::string materials = working_directory;
	materials += LIBRARY_MATERIALS_FOLDER;

	if (!Exsists(materials))
		if (!CreateFolder("Materials", false, LIBRARY_FOLDER))
			LOG("Materials folder could not be created");

	std::string textures = working_directory;
	textures += LIBRARY_TEXTURES_FOLDER;

	if (!Exsists(textures))
		if (!CreateFolder("Textures", false, LIBRARY_FOLDER))
			LOG("Textures folder could not be created");

	std::string prefabs = working_directory;
	prefabs += LIBRARY_PREFABS_FOLDER;

	if (!Exsists(prefabs))
		if (!CreateFolder("Prefabs", false, LIBRARY_FOLDER))
			LOG("Prefabs folder could not be created");

	std::string animations = working_directory;
	animations += LIBRARY_ANIMATIONS_FOLDER;

	if (!Exsists(animations))
		if (!CreateFolder("Animations", false, LIBRARY_FOLDER))
			LOG("Animations folder could not be created");

	std::string skeletons = working_directory;
	skeletons += LIBRARY_SKELETONS_FOLDER;

	if (!Exsists(skeletons))
		if (!CreateFolder("Skeletons", false, LIBRARY_FOLDER))
			LOG("Skeletons folder could not be created");

	timer.Start();

	return true;
}

UPDATE_STATUS FileSystem::Update(float dt)
{
	if (timer.ReadSec() >= UPDATE_TIME)
	{
		App->resource_manager->UpdateAssets();
		timer.Start();
	}
	return UPDATE_CONTINUE;
}

bool FileSystem::CreateFolder(const char * name, bool hidden, const char * relative_path) const
{
	bool ret = true;

	std::string new_directory(working_directory);
	if (relative_path != nullptr)
		new_directory += relative_path;
	new_directory += "\\";
	new_directory += name;

	if (CreateDirectory(new_directory.c_str(), NULL) != 0)
	{
		if (hidden)
			SetFileAttributes(new_directory.c_str(), FILE_ATTRIBUTE_HIDDEN);
		LOG("Created New Directory: %s, path: %s", name, new_directory.c_str());
	}
	else
	{
		DWORD error = GetLastError();
		if (error == ERROR_ALREADY_EXISTS)
			LOG("The specified directory already exists.");
		else if(error == ERROR_PATH_NOT_FOUND)
			LOG("One or more intermediate directories do not exist; this function will only create the final directory in the path");
		ret = false;
	}

	return ret;
}

unsigned int FileSystem::LoadFileBinary(const std::string& path, char ** buffer) const
{
	unsigned int size = 0;
	std::ifstream ifs(path, std::ios::binary);

	if (ifs.good())
	{
		if (ifs.is_open() == true)
		{
			// get length of file:
			ifs.seekg(0, ifs.end);
			int length = ifs.tellg();
			ifs.seekg(0, ifs.beg);

			// allocate memory:
			*buffer = new char[length];

			// read data as a block:
			ifs.read(*buffer, length);

			if (ifs)
			{
				LOG("Succesfully read %s", path.c_str());
				size = length;
			}
			else
				LOG("Only %i could characters where read out of %i", ifs.gcount(), length);

			ifs.close();
		}
	}
	else
		LOG("Could not load %s, error: %s", path.c_str(), strerror(errno));

	return size;
}

bool FileSystem::SaveFile(const char * buffer, unsigned int size, const char * path, const std::string& name, const char * format) const
{
	bool ret = true;
	char full_path[256];
	sprintf(full_path, "%s\\%s.%s", path, name.c_str(), format);

	std::ofstream outfile(full_path, std::ofstream::binary);

	if (outfile.good())
	{
		// write to outfile
		outfile.write(buffer, size);
		outfile.close();
	}
	else
	{
		LOG("Couldn't save file %s.%s", name.c_str(), format);
		ret = false;
	}
	return ret;
}

bool FileSystem::SaveMetaFile(const char * buffer, unsigned int size, const char * name, const char* dir) const
{
	return SaveFile(buffer, size, dir, name, "meta");
}

bool FileSystem::CopyTo(const std::string& path, const std::string& dir) const
{
	std::string name_ext(path.substr(path.find_last_of("\\") + 1));
	if (IsIn(name_ext, dir))
	{
		LOG("File %s already saved in %s", name.c_str(), dir.c_str());
		return true;
	}

	bool ret = true;
	std::string name;
	std::string extension;
	
	size_t bar = path.find_last_of("\\");
	size_t dot = path.find_last_of(".");

	if (bar != path.length() && dot != path.length())
	{
		size_t count = dot - bar - 1; // -1 to avoid dot
		name = path.substr(bar + 1, count);  //and +1 to avoid "\\"
		extension = path.substr(dot + 1); // +1 to avoid dot
	}

	unsigned int length;
	char* buffer = nullptr;

	length = LoadFileBinary(path, &buffer);

	if (length != 0 && buffer != nullptr)
		ret = SaveFile(buffer, length, dir.c_str(), name.c_str(), extension.c_str());

	delete[] buffer;

	return ret;
}

AssetDirectory* FileSystem::GenerateAssets(const std::string & directory) const
{
	DIR* dir;
	struct dirent* ent;
	AssetDirectory* asset_dir = nullptr;

	if ((dir = opendir(directory.c_str())) != NULL)
	{
		asset_dir = new AssetDirectory(directory);
		asset_dir->SetTimeStamp(GetTimeStamp(directory));
		/* Create assets for all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL)
		{
			std::string file(ent->d_name);

			if (ent->d_type == DT_DIR && file != "." && file != "..")
			{
				std::string dir_path(directory + "\\" + file);
				AssetDirectory* child_dir = GenerateAssets(dir_path);
				if (child_dir != nullptr)
					asset_dir->AddDir(child_dir);
			}
			else if (ent->d_type == DT_REG)
			{
				if (IsMeta(file))
				{
					App->import_manager->MetaLoad(file, asset_dir);
					LOG("Generated asset from %s into folder %s", file.c_str(), asset_dir->GetName().c_str());
				}
				else if (HasMeta(file, directory) == false)
				{
					std::string file_path(directory + "\\" + file);
					ImportManager::ImportClient::StraightImport(file_path, asset_dir);
				}
			}
		}
		closedir(dir);
	}
	else 
		LOG("Directory %s could not be opened", directory.c_str());

	return asset_dir;
}

bool FileSystem::Exsists(const char * path) const
{
	DWORD atributes = GetFileAttributes(path);
	if (atributes == INVALID_FILE_ATTRIBUTES)
	{
		LOG("something is wrong with the checked path");
		return false;  //something is wrong with your path!
	}

	if (atributes & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

bool FileSystem::Exsists(const std::string & path) const
{
	return Exsists(path.c_str());
}

const std::string & FileSystem::GetWorkingDirectory() const
{
	return working_directory;
}

const std::string FileSystem::GetAssets() const
{
	return working_directory + ASSETS_FOLDER;
}

const std::string FileSystem::GetTextures() const
{
	return working_directory + LIBRARY_TEXTURES_FOLDER;
}

const std::string FileSystem::GetMaterials() const
{
	return working_directory + LIBRARY_MATERIALS_FOLDER;
}

const std::string FileSystem::GetMeshes() const
{
	return working_directory + LIBRARY_MESHES_FOLDER;
}

const std::string FileSystem::GetPrefabs() const
{
	return working_directory + LIBRARY_PREFABS_FOLDER;
}

const std::string FileSystem::GetSkeletons() const
{
	return working_directory + LIBRARY_SKELETONS_FOLDER;
}

const std::string FileSystem::GetAnimations() const
{
	return working_directory + LIBRARY_ANIMATIONS_FOLDER;
}

bool FileSystem::IsMeta(const std::string & file) const
{
	return file.substr(file.find_last_of(".")) == ".meta";
}

bool FileSystem::HasMeta(const std::string & file, const std::string& directory) const
{
	DIR* dir;
	struct dirent* ent;

	if ((dir = opendir(directory.c_str())) != NULL)
	{
		/* Create assets for all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL)
		{
			std::string file_in_dir(ent->d_name);
			if (ent->d_type == DT_REG)
				if (IsMeta(file_in_dir))
					if (file_in_dir.substr(0, file_in_dir.find_last_of(".")) == file)
						return true;
		}
		closedir(dir);
	}

	return false;
}

bool FileSystem::IsIn(const std::string & file, const std::string & directory) const
{
	DIR* dir;
	struct dirent* ent;

	if ((dir = opendir(directory.c_str())) != NULL)
	{
		/* Create assets for all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL)
		{
			std::string file_in_dir(ent->d_name);
			if (ent->d_type == DT_REG)
				if (file_in_dir == file)
					return true;
		}
		closedir(dir);
	}

	return false;
}

bool FileSystem::EraseFile(const std::string & full_path) const
{
	return remove(full_path.c_str()) == 0;
}

time_t FileSystem::GetMetaTimeStamp(const std::string & file) const
{
	char* buffer = nullptr;
	char* iterator = nullptr;

	LoadFileBinary(file, &buffer);

	iterator = buffer;

	time_t ret;
	memcpy(&ret, iterator, sizeof(time_t));

	delete[] buffer;

	return ret;
}

time_t FileSystem::GetTimeStamp(const std::string & file) const
{
	struct stat result;
	if (stat(file.c_str(), &result) == 0)
		return result.st_mtime;

	return 0;
}
