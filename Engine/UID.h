#ifndef UNIQUE_IDENTIFIER
#define UNIQUE_IDENTIFIER

#include <cstring>
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

	UID(char* buffer, unsigned int length)
	{
		memcpy(uid, MD5::md5(buffer, length), 16);
	}

	void Generate(char* buffer, unsigned int length)
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
};


#endif // !UNIQUE_IDENTIFIER

