#ifndef _MODULE_SCENE_MANAGER
#define _MODULE_SCENE_MANAGER

#include <vector>
#include "glew\include\GL\glew.h"
#include "Module.h"
#include "GameObject.h"

#define CHECKERS_HEIGHT 256
#define CHECKERS_WIDTH 256

class GameObject;

class SceneManager : public Module
{
private:
	GLuint checkers_text_id;
	DRAW_MODE draw_mode;
	//std::vector<GameObject*> game_objects_vec;

	bool wireframe;
	bool normals;
	bool tris;

public:
	GameObject go;

	SceneManager(const char* name, bool start_enabled = true);
	~SceneManager();

	bool Init();

	UPDATE_STATUS Configuration(float dt);

	UPDATE_STATUS Update(float dt);

	DRAW_MODE GetDrawMode() const;

	//Primitives
	void CreateCube();
	bool create_cube = false;
};

#endif // !_MODULE_SCENE_MANAGER
