#ifndef _MODULE_SCENE_MANAGER
#define _MODULE_SCENE_MANAGER

#include <vector>
#include "glew\include\GL\glew.h"
#include "Module.h"

class GameObject;
class Camera;

class KDTreeGO;
enum TEXTURE_TYPE;

enum DRAW_MODE
{
	DM_NO_DRAW = 0,
	DM_NORMAL,
	DM_CHECKERS,
	DM_WIREFRAME,
	DM_NORMALS
};

class SceneManager : public Module
{
private:
	DRAW_MODE draw_mode;
	bool config_scene = true;
	bool wireframe;
	bool normals;
	bool polygons;
	bool debug = true;

	//Game Objects
	GameObject* root;
	GameObject* focused;
	unsigned int num_game_objects = 1;
	bool hirarchy_active = true;
	KDTreeGO* go_kdtree;

	Camera* focused_camera = nullptr;

	//GenerateGOVec() //for KDT
	//RecalculateKDT(std::vector<GameObject*>);

	void Hirarchy();
	void UpdateGameObjects();

public:
	SceneManager(const char* name, bool start_enabled = true);
	~SceneManager();

	bool Init();

	UPDATE_STATUS Configuration(float dt);

	UPDATE_STATUS Update(float dt);

	void OpenCloseConfigSceneWindow();

	void DrawMode() const;

	void EmptyScene();

	//Game Objects
	GameObject* CreateGameObject(const char* const name = nullptr);
	GameObject* CreateGameObject( GameObject* parent, const char* const name = nullptr);
	void RemoveWithChilds(GameObject* to_remove);

	void DrawKDT() const;
	bool AddToKDT( GameObject* new_go);

	void CreateCamera();
	GameObject* GetRoot() const;

	void DrawCamera();
	void SetCameraFocused(Camera* camera);

	void SetFocused(GameObject* game_obj);
	GameObject* GetFocused() const;

	void AddPrefabToFocused(GameObject* root);

	void UpdateAABBs(const math::AABB& aabb, const math::float4x4& parent_matrix) const;
};

#endif // !_MODULE_SCENE_MANAGER
