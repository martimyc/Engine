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

	UID(char* buffer, unsigned int length)
	{
		memcpy(uid, MD5::md5(buffer, length), 16);
	}

	bool operator == (const UID& cmp)
	{
		return memcmp(uid, cmp.uid, 16) == 0;
	}

	void operator = (const UID& new_id)
	{
		memcpy(uid, new_id.uid, 16);
	}
};


#endif // !UNIQUE_IDENTIFIER

