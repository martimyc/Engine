#ifndef _MODULE_OPEN_GL_TEST
#define _MODULE_OPEN_GL_TEST

#include <vector>
#include "Module.h"

class OpenGLTest: public Module
{
private:
	float triangle_vertices[9] = {
		0.0f, 0.5f, 0.0f, //top
		-0.5f, -0.5f, 0.0f, //bot-left
		0.5f, -0.5f, 0.0f //bot-right
	};
	float quad_vertices[12] = {
		50.0f, 50.0f, 0.0f,
		300.0f, 50.0f, 0.0f,
		300.0f, 300.0f, 0.0f,
		50.0f, 300.0f, 0.0f
	};
	bool wireframe = false;

public:
	OpenGLTest(Application* app, bool start_enabled = true);
	~OpenGLTest();

	UPDATE_STATUS Configuration(float dt);

	void DrawTriangle() const;
	void DrawQuad() const;
};

#endif _MODULE_OPEN_GL_TEST