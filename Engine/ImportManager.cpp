#include "Globals.h"
#include "TextureImporter.h"
#include "Application.h"
#include "SceneManager.h"
#include "FileSystem.h"
#include "Transformation.h"
#include "Texture.h"
#include "TextureImporter.h"
#include "MeshImporter.h"
#include "MaterialImporter.h"
#include "Mesh.h"
#include "ImportManager.h"

ImportManager::ImportManager(const char * name, bool start_enabled): Module(name, start_enabled)
{}

ImportManager::~ImportManager()
{}

bool ImportManager::Init()
{
	texture_importer = new TextureImporter();
	material_importer = new MaterialImporter();
	//mesh_importer = new MeshImporter();
	//scene_inporter = new ImportManager();

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool ImportManager::CleanUp()
{	
	// detach log stream
	aiDetachAllLogStreams();
	return true;
}

bool ImportManager::ImportFromPath(const std::string & path, IMPORT_TYPE type, void* imported)
{
	bool ret = true;

	std::string name(path); //this will go from full path to file name with extension in copy to assets and then to just name in load 

	if (!App->file_system->CopyToAssets(name))
	{
		LOG("Could not copy %s to assets", path.c_str());
		ret = false;
	}

	if (ret)
	{
		switch (type)
		{
		case IT_TEXTURE:
			if (texture_importer->Import(name))
			{
				imported = App->scene_manager->CreateTexture(name);
				if (!texture_importer->Load(name, *((Texture*)imported)))
				{
					LOG("Could not load %s corectlly", name.c_str());
					ret = false;
					App->scene_manager->DeleteTexture((Texture*)imported);
				}
			}
			else
			{
				LOG("Could not import %s corectlly", name.c_str());
				ret = false;
			}			
			break;
		case IT_SCENE:
			ImportScene(path);
			imported = nullptr;
			break;
		default:
			LOG("Unknown import type");
			break;
		}
	}

	return ret;
}

bool ImportManager::Load(const std::string & name, LOAD_TYPE type, void * loaded)
{
	bool ret = true;

	switch (type)
	{
	case LT_TEXTURE:
		loaded = App->scene_manager->CreateTexture(name);
		if (!texture_importer->Load(name, *((Texture*)loaded)))
		{
			LOG("Could not load %s corectlly", name.c_str());
			ret = false;
			App->scene_manager->DeleteTexture((Texture*)loaded);
		}
		break;
	case LT_SCENE:
		//LoadScene(name); not for the time beeing
		break;
	case LT_MESH:
		/*loaded = new Mesh(name.c_str());
		if (!mesh_importer->Load(name, *((Mesh*)loaded)))
		{
			LOG("Could not load %s corectlly", name.c_str());
			ret = false;
			delete loaded;
		}*/
		break;
	case LT_MATERIAL:
		loaded = App->scene_manager->CreateTexture(name);
		if (!material_importer->Load(name, *((Material*)loaded)))
		{
			LOG("Could not load %s corectlly", name.c_str());
			ret = false;
			delete loaded;
		}
		break;
	default:
		break;
	}

	return ret;
}

bool ImportManager::ImportScene(const std::string & path) const
{
	bool ret = true;

	std::string dir;
	// find the last occurrence of '.'
	size_t pos = path.find_last_of("\\");
	// make sure the poisition is valid
	if (pos != path.length())
		dir = path.substr(NULL, pos + 1);
	else
		LOG("No dir in path");

	const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr)
	{
		unsigned int previous_loaded_materials = App->scene_manager->NumMaterials();

		if (scene->HasMaterials())
		{
			//Import all materials first so that we can bind them to a mesh later when we load them

			for (unsigned int i = 0; i < scene->mNumMaterials; i++)
			{
				LOG("Importing Material %i", i);

				std::string material_name;

				if (material_importer->Import(scene->mMaterials[i], path, i, material_name))
				{
					if (LoadMaterial(material_name) == false)
					{
						LOG("Material (%i) could not be loaded correctly", i);
						ret = false;
					}
				}
				else
				{
					LOG("Material (%i) could not be imported correctly", i);
					ret = false;
				}
			}
		}
		else
			LOG("Scene has no materials");
		/*
		if (scene->HasMeshes())
		{

			// Load all meshes (for now into the same game object)
			GameObject* new_game_object = App->scene_manager->CreateGameobject();

			new_game_object->ReserveComponentSpace(scene->mNumMeshes);

			for (unsigned int i = 0; i < scene->mNumMeshes; i++)
			{
				LOG("Loading Mesh %i", i);

				Mesh* new_mesh = new_game_object->CreateMesh();
				ret = LoadMesh(scene->mMeshes[i], *new_mesh, previous_loaded_materials);

				if (ret == false)
				{
					LOG("Mesh (%i) did't load correctly", i);
					new_game_object->DeleteMesh(new_mesh);
				}

				if (scene->HasMaterials() && material_loads[scene->mMeshes[i]->mMaterialIndex] == true)
					new_mesh->SetMaterial(previous_loaded_materials + scene->mMeshes[i]->mMaterialIndex);
				else
					LOG("Material for this mesh did not load correctly");
			}
		}
		else
			LOG("More than a single mesh in scene, will Import all as one Game Object");

		aiReleaseImport(scene);*/
	}
	else
		LOG("Error loading scene %s", path);
		
	return ret;
}
/*
bool ImportManager::LoadScene(const std::string & name) const
{
	bool ret = true;

	std::string dir;
	// find the last occurrence of '.'
	size_t pos = path.find_last_of("\\");
	// make sure the poisition is valid
	if (pos != path.length())
		dir = path.substr(NULL, pos + 1);
	else
		LOG("No dir in path");

	const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr)
	{
		bool* material_loads;
		unsigned int previous_loaded_materials = App->scene_manager->NumMaterials();
		 
		if (scene->HasMaterials())
		{
			material_loads = new bool[scene->mNumMaterials];

			//Load all materials first so that we can bind them to a mesh later when we load them
			App->scene_manager->ReserveMaterialSpace(scene->mNumMaterials);

			for (unsigned int i = 0; i < scene->mNumMaterials; i++)
			{
				LOG("Loading Material %i", i);

				Material* new_material = App->scene_manager->CreateMaterial();
				ret = LoadMaterial(scene->mMaterials[i], *new_material, dir);

				if (ret == false)
				{
					LOG("Material (%i) did't load correctly", i);
					App->scene_manager->DeleteMaterial(new_material);
					material_loads[i] = false;
				}
				else
					material_loads[i] = true;
			}
		}
		else
			LOG("Scene has no materials");

		if (scene->HasMeshes())
		{

			// Load all meshes (for now into the same game object)
			GameObject* new_game_object = App->scene_manager->CreateGameobject();

			new_game_object->ReserveComponentSpace(scene->mNumMeshes);

			for (unsigned int i = 0; i < scene->mNumMeshes; i++)
			{
				LOG("Loading Mesh %i", i);

				Mesh* new_mesh = new_game_object->CreateMesh();
				ret = LoadMesh(scene->mMeshes[i], *new_mesh, previous_loaded_materials);

				if (ret == false)
				{
					LOG("Mesh (%i) did't load correctly", i);
					new_game_object->DeleteMesh(new_mesh);
				}

				if (scene->HasMaterials() && material_loads[scene->mMeshes[i]->mMaterialIndex] == true)
					new_mesh->SetMaterial(previous_loaded_materials + scene->mMeshes[i]->mMaterialIndex);
				else
					LOG("Material for this mesh did not load correctly");
			}

			new_game_object->CreateTransformation();
		}
		else
			LOG("More than a single mesh in scene, will Import all as one Game Object");

		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", path);

	return ret;
}
*/
void ImportManager::LoadCheckers()
{
	Texture* checkers = App->scene_manager->CreateTexture("Checkers");
	texture_importer->LoadCheckers(*checkers);
}
/*
bool ImportManager::LoadMesh(const aiMesh * mesh, Mesh& new_mesh, const unsigned int previous_loaded_materials) const
{
	bool ret = true;

	//This two are just in case, i see GL veriavle formats as type defs but they have fixed formats so i understeand they are typedefs here cus my architecture has the same sizes as GL but this might not always be true
	bool equal_size_floats = (sizeof(float) == sizeof(GLfloat)) ? true : false;
	bool equal_size_uints = (sizeof(unsigned int) == sizeof(GLuint)) ? true : false;

	GLuint num_vertices = mesh->mNumVertices;

	//Load Vertices
	if (mesh->HasPositions())
	{
		ret = LoadVertices(mesh, num_vertices, new_mesh, equal_size_floats, equal_size_uints);
		if (ret == false)
			LOG("Vertices did not load correctly");
	}
	else
	{
		LOG("Mesh has no vertices");
		ret = false;
	}

	// Copy faces
	if (mesh->HasFaces() && ret == true)
	{
		ret = LoadIndices(mesh, num_vertices, new_mesh, equal_size_floats, equal_size_uints);
		if (ret == false)
			LOG("Indices did not load correctly");
	}
	else
		LOG("Mesh has no faces");

	if (mesh->HasTextureCoords(0) && ret == true)
	{
		bool result = LoadTextureCoordinates(mesh, num_vertices, new_mesh, equal_size_floats, equal_size_uints);
		if (result == false)
			LOG("Texture coordinates did not load correctly");
	}
	else
		LOG("Mesh has no texture coordinates in channel 0");

	if (ret == true)
	{
		//Normals will either exist or be generated here all to 1.0f, 1.0f, 1.0f
		bool result = LoadNormals(mesh, num_vertices, new_mesh, equal_size_floats, equal_size_uints);
		if (result == false)
			LOG("Normals did not load correctly or where not generated properly");
	}

	if (mesh->HasVertexColors(0) && ret == true)
	{
		bool result = LoadColors(mesh, num_vertices, new_mesh, equal_size_floats, equal_size_uints);
		if (result == false)
			LOG("Colors did not load correctly");
	}
	else
		LOG("Mesh has no vertex colors");

	// Bones and TangentsAndBitangents not loaded yet TODO

	return ret;
}

bool ImportManager::LoadVertices(const aiMesh* mesh, const GLuint & num_vertices, Mesh & new_mesh, bool equal_size_floats, bool equal_size_uints) const
{
	bool ret = true;

	GLuint vertex_id = 0;
	GLfloat* vertices = new GLfloat[num_vertices * 3];

	if (!equal_size_floats)
	{
		LOG("Size of float not equal to GLfloat\nTransforming all values to OpenGL standard (GLfloat)\nThis might take a while");

		for (uint i = 0; i < num_vertices; i++)
		{
			//Slowest thing ever but better than nothing if sizes do not mach (may be a better way)
			vertices[i * 3] = mesh->mVertices[i][0];
			vertices[i * 3 + 1] = mesh->mVertices[i][1];
			vertices[i * 3 + 2] = mesh->mVertices[i][2];
		}
	}
	else
		memcpy(vertices, mesh->mVertices, sizeof(GLfloat) * num_vertices * 3);

	LOG("New mesh with %d vertices", num_vertices);

	glGenBuffers(1, &vertex_id);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* num_vertices * 3, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	new_mesh.SetVertices(vertex_id, num_vertices, vertices);

	return ret;
}

bool ImportManager::LoadIndices(const aiMesh* mesh, const GLuint & num_vertices, Mesh & new_mesh, bool equal_size_floats, bool equal_size_uints) const
{
	bool ret = true;

	GLuint indices_id = 0;
	GLuint num_indices = 0;
	GLuint* indices = nullptr;

	num_indices = mesh->mNumFaces * 3;
	indices = new GLuint[num_indices]; // assume each face is a triangle

	if (!equal_size_uints)
		LOG("Size of unsigned int not equal to GLuint\nTransforming all values to OpenGL standard (GLuint)\nThis might take a while");

	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		if (mesh->mFaces[i].mNumIndices != 3)
		{
			LOG("WARNING, geometry face with != 3 indices!");
			ret = false;
			break;
		}
		else
		{
			//Indices
			if (equal_size_uints)
				memcpy(&indices[i * 3], mesh->mFaces[i].mIndices, sizeof(GLuint) * 3);
			else
				for (uint j = 0; j < 3; j++)
				{
					//Slowest thing ever but better than nothing if sizes do not mach (may be a better way)
					indices[i * 3 + j] = mesh->mFaces[i].mIndices[j];
				}
		}
	}

	//Load indicies to VRAM
	glGenBuffers(1, &indices_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * num_indices, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	new_mesh.SetIndices(indices_id, num_indices, indices);

	return ret;
}

bool ImportManager::LoadTextureCoordinates(const aiMesh* mesh, const GLuint & num_vertices, Mesh & new_mesh, bool equal_size_floats, bool equal_size_uints) const
{
	bool ret = true;

	GLuint num_uv_channels = mesh->GetNumUVChannels();
	GLuint* uv_ids = new GLuint[num_uv_channels];
	GLuint* num_uv_components = new GLuint[num_uv_channels];
	GLfloat** uvs = new GLfloat*[num_uv_channels];

	for (int i = 0; i < num_uv_channels; i++)
	{
		num_uv_components[i] = mesh->mNumUVComponents[0];
		uvs[i] = new GLfloat[num_vertices * num_uv_components[i]];

		if (equal_size_floats)
		{
			if (num_uv_components[i] == 3)
				memcpy(uvs[i], mesh->mTextureCoords[i], sizeof(GLfloat) * num_vertices * num_uv_components[i]);
			else
			{
				//Slowest but better than nothing saving lots of zeroes in vram (z and maybe y coordinates), slow load but less vram usage
				for (int j = 0; j < num_vertices; j++)
					memcpy(&uvs[i][j*num_uv_components[i]], &mesh->mTextureCoords[i][j].x, sizeof(GLfloat) * num_uv_components[i]);
			}
		}
		else
		{
			LOG("Size of float not equal to GLfloat\nTransforming all values to OpenGL standard (GLfloat)\nThis might take a while");

			for (int j = 0; j < num_vertices; j++)
			{
				//Slowest thing ever but better than nothing if sizes do not mach (may be a better way)
				uvs[i][j * num_uv_components[i]] = mesh->mTextureCoords[i][j].x;
				if (num_uv_components[i] >= 2)
					uvs[i][j * num_uv_components[i] + 1] = mesh->mTextureCoords[i][j].y;
				if (num_uv_components[i] >= 3)
					uvs[i][j * num_uv_components[i] + 2] = mesh->mTextureCoords[i][j].z;
			}
		}

		glGenBuffers(1, &uv_ids[i]);
		glBindBuffer(GL_ARRAY_BUFFER, uv_ids[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * num_vertices * num_uv_components[i], uvs[i], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	new_mesh.SetUVs(num_uv_channels, num_uv_components, uv_ids, uvs);

	return ret;
}

bool ImportManager::LoadNormals(const aiMesh* mesh, const GLuint & num_vertices, Mesh & new_mesh, bool equal_size_floats, bool equal_size_uints) const
{
	bool ret = true;

	GLuint normals_id = 0;
	GLfloat* normals = new GLfloat[num_vertices * 3];

	if (mesh->HasNormals() && ret == true)
	{
		if (!equal_size_floats)
		{
			LOG("Size of float not equal to GLfloat\nTransforming all values to OpenGL standard (GLfloat)\nThis might take a while");

			for (uint i = 0; i < num_vertices; i++)
			{
				//Slowest thing ever but better than nothing if sizes do not mach (may be a better way)
				normals[i * 3] = mesh->mNormals[i][0];
				normals[i * 3 + 1] = mesh->mNormals[i][1];
				normals[i * 3 + 2] = mesh->mNormals[i][2];
			}
		}
		else
			memcpy(normals, mesh->mNormals, sizeof(GLfloat) * num_vertices * 3);
	}
	else
	{
		LOG("Mesh has no Normals even thow assimp should have generated them, all normals will be set to: 1.0f, 1.0f, 1.0f");
		memset(normals, 1.0f, num_vertices * 3);
	}

	glGenBuffers(1, &normals_id);
	glBindBuffer(GL_ARRAY_BUFFER, normals_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * num_vertices * 3, normals, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	new_mesh.SetNormals(normals_id, normals);

	return ret;
}

bool ImportManager::LoadColors(const aiMesh* mesh, const GLuint & num_vertices, Mesh & new_mesh, bool equal_size_floats, bool equal_size_uints) const
{
	bool ret = true;

	GLuint num_color_channels = mesh->GetNumColorChannels();
	GLuint* color_ids = new GLuint[num_color_channels];
	GLfloat** colors = new GLfloat*[num_color_channels];

	for (int i = 0; i < num_color_channels; i++)
	{
		colors[i] = new GLfloat[num_vertices * 4];

		if (equal_size_floats)
			memcpy(colors[i], mesh->mColors[i], sizeof(GLfloat) * num_vertices * 4);
		else
		{
			for (int j = 0; j < num_vertices; j++)
			{
				colors[i][j * 4] = mesh->mColors[i][j][0];
				colors[i][j * 4 + 1] = mesh->mColors[i][j][1];
				colors[i][j * 4 + 2] = mesh->mColors[i][j][2];
				colors[i][j * 4 + 3] = mesh->mColors[i][j][3];
			}
		}

		glGenBuffers(1, &color_ids[i]);
		glBindBuffer(GL_ARRAY_BUFFER, color_ids[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * num_vertices * 4, colors[i], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	new_mesh.SetColors(num_color_channels, color_ids, colors);

	return ret;
}
*/
Texture * ImportManager::LoadTexture(const std::string & name) const
{
	if (App->scene_manager->GetTexture(name) == nullptr)
	{
		Texture* new_texture = App->scene_manager->CreateTexture(name);
		if (texture_importer->Load(name, *new_texture) == false)
		{
			App->scene_manager->DeleteTexture(new_texture);
			LOG("Could not load texture %s", name.c_str());
			return nullptr;
		}
		return new_texture;
	}
	else
	{
		LOG("Texture already loaded");
		return App->scene_manager->GetTexture(name);
	}
}

Material* ImportManager::LoadMaterial(const std::string& name) const
{
	if (App->scene_manager->GetMaterial(name) == nullptr)
	{
		Material* new_material = App->scene_manager->CreateMaterial(name.c_str());

		if (material_importer->Load(name, *new_material) == false)
		{
			App->scene_manager->DeleteMaterial(new_material);
			LOG("Could not load material %s", name.c_str());
			return nullptr;
		}
		return new_material;
	}
	else
	{
		LOG("Material already loaded");
		return App->scene_manager->GetMaterial(name);
	}
}