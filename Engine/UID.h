#ifndef UNIQUE_IDENTIFIER
#define UNIQUE_IDENTIFIER

#include <cstring>
#include <stdio.h>
#include "MD5.h"

struct UID
{
	char uid[16];

	UID()
	{
		memset(uid, 0, 16);
	}

	UID(const UID& id)
	{
		memcpy(uid, id.uid, 16);
	}

	UID(const char* id)
	{
		memcpy(uid, id, 16);
	}

	UID(const char* buffer, unsigned int length)
	{
		memcpy(uid, MD5::md5(buffer, length), 16);
	}

	void Generate(const char* buffer, unsigned int length)
	{
		memcpy(uid, MD5::md5(buffer, length), 16);
	}

	bool IsNull() const
	{
		char tmp[16];
		memset(tmp, 0, 16);
		return uid == tmp;
	}

	bool operator == (const UID& cmp) const
	{
		return memcmp(uid, cmp.uid, 16) == 0;
	}

	void operator = (const UID& new_id)
	{
		memcpy(uid, new_id.uid, 16);
	}

	char* GetAsName() const
	{
		char name[17];
		sprintf(name, "%.*s", 16, uid);
		return name;
	}
};


#endif // !UNIQUE_IDENTIFIER

