#ifndef APPLIED_MATERIAL
#define APPLIED_MATERIAL

#include <vector>
#include "Component.h"

class Material;
class Texture;

class AppliedMaterial : public Component
{
private:
	Material* material;
	std::vector<unsigned int> uv_channels;

public:
	AppliedMaterial( Material* material, bool enabled = true);
	AppliedMaterial(const AppliedMaterial& copy);
	~AppliedMaterial();

	const unsigned int GetUVChannel(const unsigned int num_texture) const;

	const Material* GetMaterial() const;

	void Inspector();

	const std::string& GetName() const;

	void AddTexture( Texture* new_texture, unsigned int uv_channel = 0);
};

#endif // !APPLIED_MATERIAL

