#ifndef _MODULE_OPEN_GL_TEST
#define _MODULE_OPEN_GL_TEST

#include <vector>
#include "Module.h"

class OpenGLTest: public Module
{
private:
	GLfloat triangle_vertices[9] = {
		400.0f, 300.0f, 0.0f, //top
		300.0f, 50.0f, 0.0f, //bot-left
		500.0f, 50.0f, 0.0f //bot-right
	};
	GLfloat quad_vertices[12] = {
		50.0f, 50.0f, 0.0f,
		300.0f, 50.0f, 0.0f,
		300.0f, 300.0f, 0.0f,
		50.0f, 300.0f, 0.0f
	};
	GLfloat point[2] = { 0, 0 };
	bool wireframe = false;

public:
	OpenGLTest(Application* app, bool start_enabled = true);
	~OpenGLTest();

	UPDATE_STATUS Configuration(float dt);

	UPDATE_STATUS Update(float dt);

	void DrawTriangle() const;
	void DrawQuad() const;
	void Draw2DPoint() const;
};

#endif _MODULE_OPEN_GL_TEST