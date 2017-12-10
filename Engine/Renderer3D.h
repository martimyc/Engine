#ifndef _MODULE_RENDERER_3D
#define _MODULE_RENDERER_3D

#include <functional>
#include <queue>
#include <vector>
#include "Light.h"
#include "glew\include\GL\glew.h"
#include "Globals.h"
#include "Module.h"

#define MAX_LIGHTS 8

class GameObject;
class Camera;

struct CompareGOPointers
{
	bool operator()(const GameObject * go1, const GameObject * go2);
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

	const GLuint& GetTextureID();
	const GLuint& GetFrameBufferID();
	const GLuint& GetDepthID();
	uint GetWidth() const;
	uint GetHeight() const;
	uint GetPosX() const;
	uint GetPosY() const;
	void SetWidth(const uint width_);
	void SetHeight(const uint height_);
	void SetPosX(const uint x);
	void SetPosY(const uint y);

private:
	GLuint frame_buffer_id = 0;
	GLuint rendered_texture_id = 0;
	GLuint depth_render_id = 0;
	uint width = 0;
	uint height = 0;
	uint pos_x = 0;
	uint pos_y = 0;
};

class Renderer3D : public Module
{
private:
	std::priority_queue<const GameObject*, std::vector<const GameObject* >, CompareGOPointers> draw_queue;
	Light lights[MAX_LIGHTS];

	bool debug_draw;
	//TODO DebugDraw
	bool show_ray_picking = false;
	bool world_axis = true;
	bool show_grid = true;
	int grid_divisions = 10;
	float grid_line_width = 2.0f;
	float grid_size = 0.5f;
	float grid_color[3] = { 0.62745f, 0.62745f, 0.62745f };	//Grey

	int play_buttons_size_x;
	int play_buttons_size_y;

	GLuint cube_id;

	bool config_renderer = false;
	FrameBuffer* render_to_texture = nullptr;
	bool mouse_on_scene_window = true;

public:
	Renderer3D(const char* name, bool start_enabled = true);
	~Renderer3D();

private:
	void DrawWorldAxis();
	void DrawGrid();

public:
	bool Init();
	UPDATE_STATUS Configuration(float dt);
	UPDATE_STATUS PreUpdate(float dt);
	UPDATE_STATUS PostUpdate(float dt);
	bool CleanUp();

	void OpenCloseConfigRendererWindow();

	void OnResize(int width, int height);
	void OnResize(int width, int height, int prev_width, int prev_height);

	void DrawGameObject(const GameObject* game_object);

	int GetScenePosX() const;
	int GetScenePosY() const; 
	int GetSceneWidth() const;
	int GetSceneHeight() const;
	bool IsSceneWindowHovered()const;
	void ChangeCameraMatrixView();
};

#endif //_MODULE_RENDERER_3D