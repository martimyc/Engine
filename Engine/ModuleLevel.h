#ifndef _MODULE_LEVEL
#define _MODULE_LEVEL

#include <vector>
#include "Module.h"

class ModuleLevel: public Module
{
private:
	float vertices[9];
	bool wireframe = false;

public:
	ModuleLevel(Application* app, bool start_enabled = true);
	~ModuleLevel();

	UPDATE_STATUS Configuration(float dt);

	bool Init();

	void DrawTriangle();
};

#endif _MODULE_LEVEL