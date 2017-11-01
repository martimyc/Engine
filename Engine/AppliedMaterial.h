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
	AppliedMaterial(const char* name, Material* material, bool enabled = true);
	AppliedMaterial( Material* material, bool enabled = true);
	~AppliedMaterial();

	//Gets
	const unsigned int& GetNumTextures() const;
	const unsigned int GetUVChannel(const unsigned int num_texture) const;
	const Material* GetMaterial() const;

	void EnableDraw() const;
	void DisableDraw() const;

	void Inspector();
};

#endif // !APPLIED_MATERIAL
