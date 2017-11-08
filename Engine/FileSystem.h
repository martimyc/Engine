#ifndef MODULE_FILE_SYSTEM
#define MODULE_FILE_SYSTEM

#include <string>
#include "Module.h"

namespace MD5 
{
	class MD5Generator;
}

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
	unsigned int LoadFileBinary(const char* path, char** buffer);

	bool SaveFile(const char* buffer, unsigned int size, const char* relative_path, const char* name, const char* format);

	bool CopyToAssets(std::string& path);

	//Gets
	bool Exsists(const char* path) const;
	bool Exsists(const std::string& path) const;
	const std::string& GetWorkingDirectory() const;
	const std::string GetAssets() const;
	const std::string GetTextures() const;
	const std::string GetMaterials() const;
	const std::string GetMeshes() const;
};

#endif //!MODULE_FILE_SYSTEM
