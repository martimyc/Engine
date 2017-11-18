#include "Material.h"
#include "AppliedMaterial.h"

AppliedMaterial::AppliedMaterial(Material * material, bool enabled) : Component(CT_APPLIED_MATERIAL, enabled), material(material)
{
	unsigned int buff_size = material->GetNumAllTextures();
	uv_channels = new unsigned int[buff_size];
}

AppliedMaterial::~AppliedMaterial()
{
	delete[] uv_channels;
}

const unsigned int AppliedMaterial::GetUVChannel(const unsigned int num_texture) const
{
	return uv_channels[num_texture];
}

const Material * AppliedMaterial::GetMaterial() const
{
	return material;
}

void AppliedMaterial::Inspector()
{
	//TODO change to pick uv channel
	material->Inspector();
}

const std::string & AppliedMaterial::GetName() const
{
	return material->GetName();
}
