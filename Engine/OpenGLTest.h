#ifndef _MODULE_OPEN_GL_TEST
#define _MODULE_OPEN_GL_TEST

#include <vector>
#include "Module.h"

#define CHECKERS_HEIGHT 100
#define CHECKERS_WIDTH 100

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
	uint cube_indices_id;

public:
	OpenGLTest(Application* app, bool start_enabled = true);
	~OpenGLTest();

	bool Init();

	UPDATE_STATUS Configuration(float dt);

	void Draw3DPoint(const GLfloat x,const GLfloat y, const GLfloat z) const;
	void DrawDebugPoint() const;
	void DrawCubeDirectMode() const;
	void DrawCubeIndicesVertex() const;
};

#endif _MODULE_OPEN_GL_TEST