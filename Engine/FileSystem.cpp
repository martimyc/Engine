#include <windows.h>
#include <fstream>
#include "FileSystem.h"

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

	return true;
}

bool FileSystem::CreateFolder(const char * name, bool hidden, const char * relative_path)
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

unsigned int FileSystem::LoadFileBinary(const std::string& path, char ** buffer)
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

unsigned int FileSystem::LoadMetaFile(const std::string & file, char ** buffer)
{
	std::string path(GetAssets());
	path += "\\";
	path += file;

	return LoadFileBinary(path, buffer);
}

bool FileSystem::SaveFile(const char * buffer, unsigned int size, const char * relative_path, const std::string& name, const char * format)
{
	bool ret = true;

	char full_path[512];
	sprintf(full_path, "%s%s\\%s.%s", working_directory.c_str(), relative_path, name.c_str(), format);

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

bool FileSystem::SaveMetaFile(const char * buffer, unsigned int size, const char * name)
{
	return SaveFile(buffer, size, ASSETS_FOLDER, name, "meta");
}

bool FileSystem::CopyToAssets(const std::string& path)
{
	bool ret = true;
	std::string name;
	std::string extension;
	std::string dir;
	
	size_t bar = path.find_last_of("\\");
	size_t dot = path.find_last_of(".");

	if (bar != path.length() && dot != path.length())
	{
		dir = path.substr(0, bar);
		size_t count = dot - bar - 1; // -1 to avoid dot
		name = path.substr(bar + 1, count);  //and +1 to avoid "\\"
		extension = path.substr(dot + 1); // +1 to avoid dot
	}

	unsigned int length;
	char* buffer = nullptr;

	length = LoadFileBinary(path, &buffer);

	if (length != 0 && buffer != nullptr)
		ret = SaveFile(buffer, length, ASSETS_FOLDER, name.c_str(), extension.c_str());

	delete[] buffer;

	return ret;
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
