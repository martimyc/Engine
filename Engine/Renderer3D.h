#ifndef _MODULE_RENDERER_3D
#define _MODULE_RENDERER_3D

#include "Light.h"
#include "glmath.h"
#include "glew\include\GL\glew.h"
#include "Globals.h"
#include "Module.h"

#define MAX_LIGHTS 8

class GameObject;

class Renderer3D : public Module
{
private:
	std::vector<const GameObject*> draw_vec;
	bool debug_draw;
	//std::vector<GameObject*> debug_draw_vec;

public:
	Light lights[MAX_LIGHTS];
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

public:
	Renderer3D(const char* name, bool start_enabled = true);
	~Renderer3D();

	bool Init();
	UPDATE_STATUS PreUpdate(float dt);
	UPDATE_STATUS PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	void DrawGO(const GameObject* game_object);

private:
	void DrawWorldAxis();	//Leaves the glColor at blue Be careful!

};

#endif //_MODULE_RENDERER_3D