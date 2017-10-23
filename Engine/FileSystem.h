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

	unsigned int LoadFile(const char* path, char** buffer);

	bool SaveFile(const char* buffer, unsigned int size, const char* relative_path, const char* name, const char* format);

	//Gets
	bool Exsists(const char* path) const;
	bool Exsists(const std::string& path) const;
};

#endif //!MODULE_FILE_SYSTEM
