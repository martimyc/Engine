#ifndef UNIQUE_IDENTIFIER
#define UNIQUE_IDENTIFIER

#include <string>

class UID
{
private:
	char uid[16];

public:
	UID();

	UID(const UID& id);

	UID(char* iterator);

	UID(const char* buffer, unsigned int length);

	void Generate(const char* buffer, unsigned int length);

	bool IsNull() const;

	bool operator == (const UID& cmp) const;

	void operator = (const UID& new_id);

	const std::string GetAsName() const;
};


#endif // !UNIQUE_IDENTIFIER

