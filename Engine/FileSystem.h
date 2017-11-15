#ifndef MODULE_FILE_SYSTEM
#define MODULE_FILE_SYSTEM

#include <string>
#include "Module.h"

class FileSystem: public Module
{
private:
	std::string working_directory;

public:
	FileSystem(const char* name, bool start_enabled = true);
	~FileSystem();

	bool Init();

	bool CreateFolder(const char* name, bool hidden = false, const char* relative_path = nullptr);

	unsigned int LoadFileBinary(const std::string& path, char** buffer);
	unsigned int LoadMetaFile(const std::string& file, char** buffer);

	bool SaveFile(const char* buffer, unsigned int size, const char* relative_path, const char* name, const char* format);
	bool SaveMetaFile(const char* buffer, unsigned int size, const char* name);

	bool CopyToAssets(const std::string& path);

	//Gets
	bool Exsists(const char* path) const;
	bool Exsists(const std::string& path) const;
	const std::string& GetWorkingDirectory() const;
	const std::string GetAssets() const;
	const std::string GetTextures() const;
	const std::string GetMaterials() const;
	const std::string GetMeshes() const;
	const std::string GetPrefabs() const;
};

#endif //!MODULE_FILE_SYSTEM
