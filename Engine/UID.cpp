#include <cstring>
#include <stdio.h>
#include "MD5.h"
#include "UID.h"

UID::UID()
{}

UID::UID(const UID & id)
{
	memcpy(uid, id.uid, 16);
}

UID::UID(char * iterator)
{
	memcpy(uid, iterator, 16);
}

UID::UID(const char * buffer, unsigned int length)
{
	memcpy(&uid, MD5::md5(buffer, length), 16);
}

void UID::Generate(const char * buffer, unsigned int length)
{
	memcpy(&uid, MD5::md5(buffer, length), 16);
}

bool UID::IsNull() const
{
	char cmp[16];
	memset(cmp, 0, 16);

	return memcmp(cmp, uid, 16) == 0;
}

bool UID::operator==(const UID & cmp) const
{
	return memcmp(cmp.uid, uid, 16) == 0;
}

void UID::operator=(const UID & new_id)
{
	memcpy(uid, new_id.uid, 16);
}

const std::string UID::GetAsName() const
{
	unsigned long long first;
	unsigned long long second;
	memcpy(&first, uid, 8);
	memcpy(&second, uid + 8, 8);

	char str_first[255];
	char str_second[255];

	sprintf(str_first,"%llx", first);
	sprintf(str_second, "%llx", second);
	std::string name (str_first);
	name += str_second;
	return name;
}
