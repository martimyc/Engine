#ifndef _MODULE_OPEN_GL_TEST
#define _MODULE_OPEN_GL_TEST

#include <vector>
#include "Module.h"

#define CHECKERS_HEIGHT 10
#define CHECKERS_WIDTH 10

class OpenGLTest: public Module
{
private:

	bool wireframe = false;
	bool debug_point = false;

	GLfloat x = 0.0f;
	GLfloat y = 0.0f;
	GLfloat z = 0.0f;

	GLuint img_id;

	uint cube_id;
	float cube_vert[8 * 3] =
	{
		0.f, 1.f, 0.f,		//0
		0.f, 0.f, 0.f,		//1
		0.f, 0.f, 1.f,		//2
		0.f, 1.f, 1.f,		//3
		1.f, 1.f, 1.f,		//4
		1.f, 0.f, 1.f,		//5
		1.f, 1.f, 0.f,		//6
		1.f, 0.f, 0.f		//7
	};

	uint cube_indices_id;
	uint cube_indices[36] =
	{
		1,2,0, 0,2,3,	//Left
		4,3,2, 4,2,5,	//Front
		7,1,0, 7,0,6,	//Back
		1,7,2, 7,5,2,	//Bottom
		0,4,6, 0,3,4,	//Top
		7,6,4, 7,6,5	//Right
	};

public:
	OpenGLTest(Application* app, bool start_enabled = true);
	~OpenGLTest();

	bool Init();

	UPDATE_STATUS Configuration(float dt);
	UPDATE_STATUS Update(float dt);

	void Draw3DPoint(const GLfloat x,const GLfloat y, const GLfloat z) const;
	void DrawDebugPoint() const;
	void DrawCubeDirectMode() const;
};

#endif _MODULE_OPEN_GL_TEST