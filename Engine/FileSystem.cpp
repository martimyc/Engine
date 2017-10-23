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
	library += "\\Library";

	if (!Exsists(library))
		if (!CreateFolder("Library", true))
			LOG("Library folder could not be created");

	std::string meshes = library;
	meshes += "\\Meshes";

	if (!Exsists(meshes))
		if (!CreateFolder("Meshes"))
			LOG("Meshes folder could not be created");

	std::string materials = library;
	materials += "\\Materials";

	if (!Exsists(materials))
		if (!CreateFolder("Materials"))
			LOG("Materials folder could not be created");

	std::string textures = library;
	textures += "\\Textures";

	if (!Exsists(meshes))
		if (!CreateFolder("Textures"))
			LOG("Textures folder could not be created");

	return true;
}

bool FileSystem::CreateFolder(const char * name, bool hidden, const char * relative_path)
{
	bool ret = true;

	std::string new_directory(working_directory);
	new_directory += "\\";
	if (relative_path != nullptr)
	{
		new_directory += relative_path;
		new_directory += "\\";
	}
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

unsigned int FileSystem::LoadFile(const char * path, char ** buffer)
{
	unsigned int size = 0;
	std::ifstream ifs(path, std::ifstream::binary);

	if (ifs.good())
		if (ifs.is_open() == true)
		{
			// get length of file:
			ifs.seekg(0, ifs.end);
			int length = ifs.tellg();
			ifs.seekg(0, ifs.beg);

			// allocate memory:
			char * buffer = new char[length];

			// read data as a block:
			ifs.read(buffer, length);

			if (ifs)
			{
				LOG("Succesfully read %s", path);
				size = length;
			}
			else
				LOG("Only %i could characters where read out of %i", ifs.gcount(), length);

			ifs.close();
		}

	return size;
}

bool FileSystem::SaveFile(const char * buffer, unsigned int size, const char * relative_path, const char * name, const char * format)
{
	bool ret = true;

	std::string full_path(working_directory);
	full_path += relative_path;
	full_path += name;
	full_path += ".";
	full_path += "dds";

	std::ofstream outfile(full_path.c_str(), std::ofstream::binary);

	if (outfile.good())
	{
		// write to outfile
		outfile.write(buffer, size);
		outfile.close();
	}
	else
	{
		LOG("Couldn't save file %s.%s", name, format);
		ret = false;
	}
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
