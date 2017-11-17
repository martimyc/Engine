#ifndef APPLIED_MATERIAL
#define APPLIED_MATERIAL

#include "Component.h"

class Material;

class AppliedMaterial : public Component
{
private:
	Material* material;
	unsigned int* uv_channels;

public:
	AppliedMaterial( Material* material, bool enabled = true);
	~AppliedMaterial();

	const unsigned int GetUVChannel(const unsigned int num_texture) const;

	const Material* GetMaterial() const;

	void Inspector();

	const std::string& GetName() const;
};

#endif // !APPLIED_MATERIAL

