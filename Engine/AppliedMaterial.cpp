#include "Material.h"
#include "AppliedMaterial.h"

AppliedMaterial::AppliedMaterial(const char * name, Material* material, bool enabled): Component(CT_APPLIED_MATERIAL,name, enabled), material(material)
{
	uv_channels = new unsigned int[material->GetNumTextures()];
	memset(uv_channels, 0, material->GetNumTextures() * sizeof(unsigned int));
}

AppliedMaterial::AppliedMaterial(Material * material, bool enabled) : Component(CT_APPLIED_MATERIAL, material->GetName(), enabled), material(material)
{
	uv_channels = new unsigned int[material->GetNumTextures()];
	memset(uv_channels, 0, material->GetNumTextures() * sizeof(unsigned int));
}

AppliedMaterial::~AppliedMaterial()
{
	delete[] uv_channels;
}

const unsigned int & AppliedMaterial::GetNumTextures() const
{
	return material->GetNumTextures();
}

const unsigned int AppliedMaterial::GetUVChannel(const unsigned int num_texture) const
{
	return uv_channels[num_texture];
}

const Material * AppliedMaterial::GetMaterial() const
{
	return material;
}

void AppliedMaterial::EnableDraw() const
{
	material->EnableDraw();
}

void AppliedMaterial::DisableDraw() const
{
	material->DisableDraw();
}

void AppliedMaterial::Inspector()
{
	//TODO change to pick uv channel
	material->Inspector();
}
