#ifndef MODULE_FILE_SYSTEM
#define MODULE_FILE_SYSTEM

#include <string>
#include "Module.h"

class AssetDirectory;

class FileSystem: public Module
{
private:
	std::string working_directory;

public:
	FileSystem(const char* name, bool start_enabled = true);
	~FileSystem();

	bool Init();

	bool Start();

	bool CreateFolder(const char* name, bool hidden = false, const char* relative_path = nullptr) const;

	unsigned int LoadFileBinary(const std::string& path, char** buffer) const;

	bool SaveFile(const char* buffer, unsigned int size, const char* relative_path, const std::string& name, const char* format) const;
	bool SaveMetaFile(const char* buffer, unsigned int size, const char* name, const char* dir) const;

	bool CopyTo(const std::string& path, const std::string& dir) const;

	AssetDirectory* GenerateAssets(const std::string& directory) const;

	//Gets
	bool Exsists(const char* path) const;
	bool Exsists(const std::string& path) const;
	const std::string& GetWorkingDirectory() const;
	const std::string GetAssets() const;
	const std::string GetTextures() const;
	const std::string GetMaterials() const;
	const std::string GetMeshes() const;
	const std::string GetPrefabs() const;

	bool IsMeta(const std::string& file) const;
	bool HasMeta(const std::string& file, const std::string& directory) const;

	bool IsIn(const std::string& file, const std::string& directory) const;
};

#endif //!MODULE_FILE_SYSTEM
