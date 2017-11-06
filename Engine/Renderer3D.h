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

class GameObject;
class Camera;

struct CompareGOPointers
{
	bool operator()(const GameObject* m1, const GameObject* m2);
};

//Renders the scene into a texture so we can put it into a ImGui Window.
class FrameBuffer
{
public:
	FrameBuffer();
	~FrameBuffer();

	void CreateFrameBuffer(const int width, const int height);
	void BindFrameBuffer();
	void UnBindFrameBuffer();

	GLuint GetTextureID();
	const uint GetWidth();
	const uint GetHeight();
	void SetWidth(const uint _width);
	void SetHeight(const uint _height);

private:
	GLuint frame_buffer_id = 0;
	GLuint rendered_texture_id = 0;
	GLuint depth_render_id;
	uint width = 0;
	uint height = 0;
};

class Renderer3D : public Module
{
private:
	std::priority_queue<const GameObject*, std::vector<const GameObject* >, CompareGOPointers> draw_queue;
	bool debug_draw;
	//TODO DebugDraw
	bool world_axis = true;
	bool show_grid = true;
	int grid_divisions = 10;
	float grid_line_width = 2.0f;
	float grid_size = 0.5f;
	float grid_color[3] = { 0.62745f, 0.62745f, 0.62745f };	//Grey
	GLuint cube_id;

	bool config_renderer = true;
	FrameBuffer* render_to_texture = nullptr;

	void DrawWorldAxis();
	void DrawGrid();

public:
	Light lights[MAX_LIGHTS];
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	Renderer3D(const char* name, bool start_enabled = true);
	~Renderer3D();

	bool Init();
	UPDATE_STATUS Configuration(float dt);
	UPDATE_STATUS PreUpdate(float dt);
	UPDATE_STATUS PostUpdate(float dt);
	bool CleanUp();

	void OpenCloseConfigRendererWindow();

	void OnResize(int width, int height);

	void DrawGameObject(const GameObject* game_object);

	void Anisotrophy();
};

#endif //_MODULE_RENDERER_3D