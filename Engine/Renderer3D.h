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