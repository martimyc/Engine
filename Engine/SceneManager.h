#ifndef _MODULE_SCENE_MANAGER
#define _MODULE_SCENE_MANAGER

#include "Module.h"

#define CHECKERS_HEIGHT 256
#define CHECKERS_WIDTH 256

class GameObject;

class SceneManager : public Module
{
private:
	GameObject* go = nullptr;
	GLuint checkers_text_id = 0;

public:
	SceneManager(const char* name, bool start_enabled = true);
	~SceneManager();

	bool Init();

	UPDATE_STATUS Update(float dt);

	void DrawGO() const;

	void SetGO(GameObject* ptr);
};

#endif // !_MODULE_SCENE_MANAGER
