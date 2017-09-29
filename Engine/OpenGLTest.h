#ifndef _MODULE_OPEN_GL_TEST
#define _MODULE_OPEN_GL_TEST

#include <vector>
#include "Module.h"

class OpenGLTest: public Module
{
private:
	float triangle_vertices[9];
	float quad_vertices[12];
	bool wireframe = false;

public:
	OpenGLTest(Application* app, bool start_enabled = true);
	~OpenGLTest();

	UPDATE_STATUS Configuration(float dt);

	bool Init();

	void DrawTriangle() const;
	void DrawQuad() const;
};

#endif _MODULE_OPEN_GL_TEST