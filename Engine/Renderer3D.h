#ifndef _MODULE_RENDERER_3D
#define _MODULE_RENDERER_3D

#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

class Renderer3D : public Module
{
private:
	void DrawCubeWithVertexArrays();
	void DrawWorldAxis();	//Leaves the glColor at blue Be careful!

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