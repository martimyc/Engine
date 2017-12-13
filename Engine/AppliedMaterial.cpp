#include "Material.h"
#include "ResourceManager.h"
#include "Application.h"
#include "AppliedMaterial.h"

AppliedMaterial::AppliedMaterial(Material * material, const GameObject* const go, bool enabled) : Component(CT_APPLIED_MATERIAL, go, enabled), material(material)
{
	unsigned int buff_size = material->GetNumAllTextures();

	for (int i = 0; i < buff_size; i++)
		uv_channels.push_back(0);
}

AppliedMaterial::AppliedMaterial(const AppliedMaterial & copy, const GameObject* const go): Component(CT_APPLIED_MATERIAL, go, copy.enabled)
{
	material = App->resource_manager->UseMaterial(copy.material->GetUID(), go);

	for (std::vector<unsigned int>::const_iterator it = copy.uv_channels.begin(); it != copy.uv_channels.end(); ++it)
		uv_channels.push_back(*it);
}

AppliedMaterial::~AppliedMaterial()
{
	uv_channels.clear();
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

void AppliedMaterial::AddTexture(Texture * new_texture, unsigned int uv_channel)
{
	material->AddTexture(new_texture, TT_DIFFUSE);
	uv_channels.push_back(uv_channel);
}

