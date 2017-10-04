#ifndef _MODULE_RENDERER_3D
#define _MODULE_RENDERER_3D

#include "Light.h"
#include "glmath.h"
#include "glew\include\GL\glew.h"
#include "Globals.h"
#include "Module.h"

#define MAX_LIGHTS 8

class Renderer3D : public Module
{
private:
	void DrawCubeWithVertexArrays();
	void DrawWorldAxis();	//Leaves the glColor at blue Be careful!
	GLuint img_id;
	uint cube_id;

	float cube_vert[36 * 3] =
	{
		//Left	
		0.f, 0.f, 0.f,
		0.f, 0.f, 1.f,
		0.f, 1.f, 0.f,

		0.f, 1.f, 0.f,
		0.f, 0.f, 1.f,
		0.f, 1.f, 1.f,

		//Front
		1.f, 1.f, 1.f,
		0.f, 1.f, 1.f,
		0.f, 0.f, 1.f,

		1.f, 1.f, 1.f,
		0.f, 0.f, 1.f,
		1.f, 0.f, 1.f,

		//Back
		1.f, 0.f, 0.f,
		0.f, 0.f, 0.f,
		0.f, 1.f, 0.f,

		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		1.f, 1.f, 0.f,

		//Bottom
		0.f, 0.f, 0.f,
		1.f, 0.f, 0.f,
		0.f, 0.f, 1.f,

		1.f, 0.f, 0.f,
		1.f, 0.f, 1.f,
		0.f, 0.f, 1.f,

		//Top
		0.f, 1.f, 0.f,
		1.f, 1.f, 1.f,
		1.f, 1.f, 0.f,

		0.f, 1.f, 0.f,
		0.f, 1.f, 1.f,
		1.f, 1.f, 1.f,

		//Right
		1.f, 0.f, 0.f,
		1.f, 1.f, 0.f,
		1.f, 1.f, 1.f,

		1.f, 0.f, 0.f,
		1.f, 1.f, 1.f,
		1.f, 0.f, 1.f,
	};

public:
	Renderer3D(Application* app, bool start_enabled = true);
	~Renderer3D();

	bool Init();
	UPDATE_STATUS PreUpdate(float dt);
	UPDATE_STATUS PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

public:
	Light lights[MAX_LIGHTS];
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
};

#endif //_MODULE_RENDERER_3D