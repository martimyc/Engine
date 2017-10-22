#ifndef _MODULE_RENDERER_3D
#define _MODULE_RENDERER_3D

#include <functional>
#include <queue>
#include <vector>
#include "Light.h"
#include "glmath.h"
#include "glew\include\GL\glew.h"
#include "Globals.h"
#include "Module.h"

#define MAX_LIGHTS 8

class Mesh;

struct CompareMeshPointers
{
	bool operator()(const Mesh* m1, const Mesh* m2);	
};

class Renderer3D : public Module
{
private:
	std::priority_queue<Mesh*, std::vector<Mesh* >, CompareMeshPointers> draw_queue;
	bool debug_draw;
	//TODO DebugDraw
	bool world_axis = true;
	bool show_grid = true;
	int grid_divisions = 10;
	float grid_line_width = 2.0f;
	float grid_size = 0.5f;
	float grid_color[3] = { 0.62745f, 0.62745f, 0.62745f };	//Grey
	GLuint cube_id;

public:
	Light lights[MAX_LIGHTS];
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

public:
	Renderer3D(const char* name, bool start_enabled = true);
	~Renderer3D();

	bool Init();
	UPDATE_STATUS Configuration(float dt);
	UPDATE_STATUS PreUpdate(float dt);
	UPDATE_STATUS PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	void DrawMesh(Mesh* mesh);

	void Anisotrophy();

private:
	void DrawWorldAxis();	//Leaves the glColor at blue Be careful!
	void DrawGrid(); //need a plane not a grid

};

#endif //_MODULE_RENDERER_3D