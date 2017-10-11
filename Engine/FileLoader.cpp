#include "FileLoader.h"
#include "glew\include\GL\glew.h"
#include "Application.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Mesh.h"

FileLoader::FileLoader(const char * name, bool start_enabled) : Module(name, start_enabled)
{}

FileLoader::~FileLoader()
{}

bool FileLoader::Init()
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool FileLoader::CleanUp()
{
	// detach log stream
	aiDetachAllLogStreams();
	return true;
}

bool FileLoader::LoadScene(const char * path)
{
	bool ret = true;

	char* full_path = "";
	const Mesh* mesh = nullptr;

	const aiScene* scene = aiImportFile(full_path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (uint i = 0; i < scene->mNumMeshes; i++)
		{
			const Mesh* new_m = LoadMesh(scene->mMeshes[i]);

			if (new_m != nullptr)
				mesh = new_m;

			if (ret == false)
			{
				LOG("Mesh(%i) did't load correctly", i);
				break;
			}
		}
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", full_path);

	GameObject* go = new GameObject(mesh);
	App->scene_manager->SetGO(go);

	return ret;
}

const Mesh * FileLoader::LoadMesh(const aiMesh * mesh)
{
	GLuint vertex_id = 0;
	GLuint num_indices = 0;
	GLuint indices_id = 0;
	GLuint num_vertices = 0;
	GLuint normals_id = 0;
	GLuint num_uv = 0;
	GLuint uv_id = 0;

	//Vertices
	num_vertices = mesh->mNumVertices;
	glGenBuffers(1, (GLuint*) &(vertex_id));
	glBindBuffer(GL_ARRAY_BUFFER, vertex_id);

	float* vertices = new float[num_vertices * 3];
	memcpy(vertices, mesh->mVertices, sizeof(float) * num_vertices * 3);
	LOG("New mesh with %d vertices", num_vertices);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * num_vertices, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// copy faces
	if (mesh->HasFaces())
	{
		num_indices = mesh->mNumFaces * 3;
		uint* indices = new uint[num_indices]; // assume each face is a triangle

		for (uint i = 0; i < mesh->mNumFaces; ++i)
		{
			if (mesh->mFaces[i].mNumIndices != 3)
				LOG("WARNING, geometry face with != 3 indices!");
			else
				memcpy(&indices[i * 3], mesh->mFaces[i].mIndices, 3 * sizeof(uint));
		}

		//Load indicies to VRAM
		glGenBuffers(1, (GLuint*) &(indices_id));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	Mesh* ret = new Mesh(vertex_id, num_indices, indices_id, num_vertices, normals_id, num_uv, uv_id);
	return ret;
}
