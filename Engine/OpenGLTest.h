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

	GLfloat triangle_strip[15] = {
		520.0f, 400.0f, 0.0f,
		600.0f, 600.0f, 0.0f,
		1000.0f, 350.0f, 0.0f,
		1000.0f, 750.0f, 0.0f,
		1100.0f, 620.0f, 0.0f
	};

	GLfloat triangle_fan [15] = {
		320, 240, 0,
		370, 290, 0,
		420, 240, 0,
		370, 190, 0,
		200, 100, 0
	};

	GLfloat triangle_cube[36 * 3] = {
		// Cara 1
		0.0f, 1.0f, 0.0f, // Triangle 1
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,		
		0.0f, 1.0f, 0.0f,  // Triangle 2
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,	
		// Cara 2
		1.0f, 1.0f, 0.0f, // Triangle 1	
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, // Triangle 2
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		// Cara 3
		1.0f, 1.0f, 1.0f, // Triangle 1
		1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, // Triangle 2
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		// Cara 4
		0.0f, 1.0f, 1.0f, // Triangle 1		
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 1.0f, // Triangle 2	
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		// Cara 5
		0.0f, 1.0f, 1.0f, // Triangle 1
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, // Triangle 2
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		// Cara 6
		0.0f, 0.0f, 1.0f, // Triangle 1
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, // Triangle 2
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f
	};
	
	GLfloat quad_strip[27] = { //order change from quad
		50.0f, 50.0f, 0.0f,
		300.0f, 50.0f, 0.0f,
		50.0f, 300.0f, 0.0f,
		300.0f, 300.0f, 0.0f,	
		320.0f, 150.0f, 0.0f,
		350.0f, 200.0f, 0.0f,
		400.0f, 50.0f, 0.0f,
		450.0f, 120.0f, 0.0f
	};

	GLint circle_sides = 3;
	
	bool wireframe = false;

public:
	OpenGLTest(Application* app, bool start_enabled = true);
	~OpenGLTest();

	UPDATE_STATUS Configuration(float dt);

	UPDATE_STATUS Update(float dt);

	void DrawTriangle() const;
	void DrawQuad() const;
	void Draw2DPoint(const float x,const float y) const;
	void DrawLine() const;
	void DrawPolygon() const;
	void DrawTriangleStrip() const;
	void DrawTriangleFan() const;
	void DrawCircle(const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat radius, const GLint number_of_sifes) const;
	void DrawHollowCircle(const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat radius, const GLint number_of_sides) const;
	void DrawQuadStrip() const;
	void DrawCubeDirectMode() const;
};

#endif _MODULE_OPEN_GL_TEST