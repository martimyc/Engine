#ifndef _MODULE_SCENE_MANAGER
#define _MODULE_SCENE_MANAGER

#include <vector>
#include "glew\include\GL\glew.h"
#include "Tree.h"
#include "Module.h"

class GameObject;
class Material;
class Texture;
class vec3;

enum DRAW_MODE
{
	DM_NO_DRAW = 0,
	DM_NORMAL,
	DM_CHECKERS,
	DM_WIREFRAME,
	DM_NORMALS
};

class SceneManager : public Module
{
private:
	DRAW_MODE draw_mode;
	Tree game_objects;
	std::vector<Material*> materials;

	bool wireframe;
	bool normals;
	bool polygons;

	int current_mesh = 0;
	unsigned int next_material = 0;

public:
	SceneManager(const char* name, bool start_enabled = true);
	~SceneManager();

	bool Init();
	bool CleanUp();

	UPDATE_STATUS Configuration(float dt);

	UPDATE_STATUS Update(float dt);

	void DrawMode() const;

	void ApplyToMaterial(Texture* new_text, int material);

	void ReserveMaterialSpace(const GLuint& num_materials);
	//void ReserveGameObjectSpace(const GLuint& num_materials); //TODO

	bool DrawNormals() const;
	unsigned int NumMaterials() const;

	Material* GetMaterial(unsigned int pos) const;
	bool HasMaterials() const;
	void CalculateDistanceToObj(const GameObject* go, vec3& center, float& x_dist, float& y_dist, float& z_dist) const;
	const GameObject* GetFocused() const;

	void EmptyScene();
	GameObject* CreateGameobject(const char* const name = nullptr);
	Material* CreateMaterial(const char* const name = nullptr);
	void DeleteMaterial(const Material* to_delete);
};

#endif // !_MODULE_SCENE_MANAGER
