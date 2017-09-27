#ifndef _MODULE_OPEN_GL_TEST
#define _MODULE_OPEN_GL_TEST

#include <vector>
#include "Module.h"

class OpenGLTest: public Module
{
private:
	float vertices[9];
	bool wireframe = false;

public:
	OpenGLTest(Application* app, bool start_enabled = true);
	~OpenGLTest();

	UPDATE_STATUS Configuration(float dt);

	bool Init();

	void DrawTriangle();
};

#endif _MODULE_OPEN_GL_TEST