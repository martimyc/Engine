#ifndef _MODULE_SCENE_MANAGER
#define _MODULE_SCENE_MANAGER

#include <vector>
#include "glew\include\GL\glew.h"
#include "Module.h"

#define CHECKERS_HEIGHT 256
#define CHECKERS_WIDTH 256

class GameObject;
class Material;
class Texture;
class vec3;
class Texture;
class Tree;
enum TEXTURE_TYPES;

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
	Tree* game_objects;

	//materials
	std::vector<Material*> materials;
	bool assets_enable = true;
	unsigned int selected_material = 0;

	bool config_scene = true;
	bool wireframe;
	bool normals;
	bool polygons;

	int current_mesh = 0;
	unsigned int next_material = 0;
	unsigned int material_priority = 0;

	//Textures
	std::vector<Texture*> textures;
	int texture_to_draw = 0;
	bool debug_textures = false;
	int current_material = 0;
	unsigned int selected_texture = 0;

public:
	SceneManager(const char* name, bool start_enabled = true);
	~SceneManager();

	bool Init();
	bool CleanUp();

	UPDATE_STATUS Configuration(float dt);
	UPDATE_STATUS MaterialsConfiguration(float dt);
	UPDATE_STATUS TexturesConfiguration(float dt);


	UPDATE_STATUS Update(float dt);

	void OpenCloseConfigSceneWindow();

	void DrawMode() const;

	void ApplyToMaterial(Texture* new_text, int material);

	void ReserveMaterialSpace(const GLuint& num_materials);
	//void ReserveGameObjectSpace(const GLuint& num_materials); //TODO

	bool DrawNormals() const;
	unsigned int NumMaterials() const;

	//Materials
	Material* GetMaterial(unsigned int pos) const;
	Material* GetMaterial(const std::string& name) const;
	void DeleteMaterial(Material* material_to_delete);
	bool HasMaterials() const;
	void CalculateDistanceToObj(const GameObject* go, vec3& center, float& x_dist, float& y_dist, float& z_dist) const;
	const GameObject* GetFocused() const;

	void EmptyScene();
	GameObject* CreateGameObject(const char* const name = nullptr);
	Material* CreateMaterial(const char* const name = nullptr);
	void DeleteMaterial(const Material* to_delete);

	//Textures
	Texture* GetTexture(unsigned int i) const;
	Texture* GetTexture(const std::string& name) const;
	void DeleteTexture(Texture* texture_to_delete);
	void AddTexture(Texture* new_texture);
	void EmptyTextures();
	const int GetTextureToDraw() const;
	bool DebugTextures() const;
	bool Exsists(const std::string& path) const;
	void DrawTexture(unsigned int num_texture) const;

	Texture* CreateTexture(const std::string& name, const TEXTURE_TYPES type = (TEXTURE_TYPES)1, const GLenum dimensions = GL_TEXTURE_2D, const GLuint& id = 0); //(TEXTURE_TYPES)1 == TT_DIFFUSE wich can not be forward declared

	//Scene
	//bool LoadScene(const std::string& path) const;
};

#endif // !_MODULE_SCENE_MANAGER
