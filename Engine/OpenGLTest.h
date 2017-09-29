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
	GLfloat line[6] = {
		550.0f, 50.0f, 0.0f,
		600.0f, 300.0f, 0.0f
	};

	GLfloat polygon[15] = {
	20.0f, 400.0f, 0.0f,
	100.0f, 400.0f, 0.0f,
	500.0f, 350.0f, 0.0f,
	320.0f, 310.0f, 0.0f,
	40.0f, 340.0f, 0.0f
	};

	GLenum poly_draw_mode = GL_FILL;
	
	bool wireframe = false;

public:
	OpenGLTest(Application* app, bool start_enabled = true);
	~OpenGLTest();

	UPDATE_STATUS Configuration(float dt);

	UPDATE_STATUS Update(float dt);

	void DrawTriangle() const;
	void DrawQuad() const;
	void Draw2DPoint() const;
	void DrawLine() const;
	void DrawPolygon() const;
};

#endif _MODULE_OPEN_GL_TEST