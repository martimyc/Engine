#ifndef MATERIAL
#define MATERIAL

#include <string>
#include <vector>
#include "glew\include\GL\glew.h"
#include "Resource.h"

class GameObject;
class Texture;

enum TEXTURE_TYPE
{
	TT_NO_TYPE = 0,
	TT_DIFFUSE,
	TT_SPECULAR,
	TT_AMBIENT,
	TT_EMISSIVE,
	TT_HEIGHT,
	TT_NORMALS,
	TT_SHININESS,
	TT_OPACITY,
	TT_DISPLACEMENT,
	TT_LIGHTMAP,
	TT_REFLECTION
};

struct MaterialSource
{
	std::vector<Texture*> diffuse_textures;
	std::vector<Texture*> specular_textures;
	std::vector<Texture*> ambient_textures;
	std::vector<Texture*> emissive_textures;
	std::vector<Texture*> height_textures;
	std::vector<Texture*> normals_textures;
	std::vector<Texture*> shininess_textures;
	std::vector<Texture*> opacity_textures;
	std::vector<Texture*> displacement_textures;
	std::vector<Texture*> lightmap_textures;
	std::vector<Texture*> reflection_textures;

	bool edit_name = false;
	unsigned int priority;

	MaterialSource(unsigned int priority);
	~MaterialSource();

	void ChangePriority(unsigned int new_priority);

	unsigned int GetNumAllTextures() const;
	unsigned int GetNumTextures(TEXTURE_TYPE type) const;

	unsigned int GetPriority() const;

	void AddTexture(Texture* new_text, TEXTURE_TYPE type);
	void ReserveVec(unsigned int size, TEXTURE_TYPE type);
	void ResizeVec(unsigned int size, TEXTURE_TYPE type);

	void EnableDraw() const;
	void DisableDraw() const;

	void Inspector();

	//Inspectors by textures
	void InspectorDiffuse();
	void InspectorSpecular();
	void InspectorAmbient();
	void InspectorEmissive();
	void InspectorHeight();
	void InspectorNormals();
	void InspectorShininess();
	void InspectorOpacity();
	void InspectorDisplacement();
	void InspectorLightMap();
	void InspectorReflection();
};

class Material : public Resource
{
private:
	MaterialSource* source;

public:
	Material(const std::string name, const UID& uid);
	Material(const std::string name, MaterialSource* source);
	Material(const Material& copy);
	~Material();

	void ChangePriority(unsigned int new_priority);

	unsigned int GetNumAllTextures() const;
	unsigned int GetNumTextures(TEXTURE_TYPE type) const;

	unsigned int GetPriority() const;

	void AddTexture(Texture* new_text, TEXTURE_TYPE type);
	void ReserveVec(unsigned int size, TEXTURE_TYPE type);
	void ResizeVec(unsigned int size, TEXTURE_TYPE type);

	void EnableDraw() const;
	void DisableDraw() const;

	bool Inspector();

	bool IsLoaded() const;

	void SetSource(MaterialSource* source);
};

#endif // !MATERIAL

