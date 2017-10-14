#ifndef _MODULE_SCENE_MANAGER
#define _MODULE_SCENE_MANAGER

#include <vector>
#include "glew\include\GL\glew.h"
#include "Module.h"

#define CHECKERS_HEIGHT 256
#define CHECKERS_WIDTH 256

class GameObject;
class Material;

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
	GLuint checkers_text_id;
	DRAW_MODE draw_mode;
	//std::vector<GameObject*> game_objects;
	std::vector<Material*> materials;

	bool wireframe;
	bool normals;
	bool polygons;

public:
	GameObject* game_object = nullptr;

	SceneManager(const char* name, bool start_enabled = true);
	~SceneManager();

	bool Init();
	bool CleanUp();

	UPDATE_STATUS Configuration(float dt);

	UPDATE_STATUS Update(float dt);

	void DrawMode() const;

	void AddMaterial(Material* new_material);

	//Primitives
	void CreateCube();
	bool create_cube = false;
};

#endif // !_MODULE_SCENE_MANAGER
