#include "MeshLoader.h"
#include "glew\include\GL\glew.h"
#include "Application.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Mesh.h"

MeshLoader::MeshLoader(const char * name, bool start_enabled) : Module(name, start_enabled)
{}

MeshLoader::~MeshLoader()
{}

bool MeshLoader::Init()
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool MeshLoader::CleanUp()
{
	// detach log stream
	aiDetachAllLogStreams();
	return true;
}

bool MeshLoader::LoadScene(const char * path)
{
	bool ret = true;

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		
		if (scene->mNumMeshes > 1)
			LOG("More than a single mesh in scene, will Import all as one Game Object");

		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			Mesh* new_m = new Mesh();
			ret = LoadMesh(scene->mMeshes[i], *new_m);

			if (ret == false)
				LOG("Mesh(%i) did't load correctly", i);
			else
				App->scene_manager->go.AddComponent(new_m);
		}
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", path);

	return ret;
}

bool MeshLoader::LoadMesh(const aiMesh * mesh, Mesh& new_mesh)
{
	bool ret = true;

	//Load Vertices
	new_mesh.num_vertices = mesh->mNumVertices;
	glGenBuffers(1, &new_mesh.vertex_id);
	glBindBuffer(GL_ARRAY_BUFFER, new_mesh.vertex_id);

	new_mesh.vertices = new float[new_mesh.num_vertices * 3];
	memcpy(new_mesh.vertices, mesh->mVertices, sizeof(float) * new_mesh.num_vertices * 3);
	LOG("New mesh with %d vertices", new_mesh.num_vertices);

	glBufferData(GL_ARRAY_BUFFER, sizeof(new_mesh.vertices), new_mesh.vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Copy faces
	if (mesh->HasFaces())
	{
		new_mesh.num_indices = mesh->mNumFaces * 3;
		new_mesh.indices = new unsigned int[new_mesh.num_indices]; // assume each face is a triangle

		for (uint i = 0; i < mesh->mNumFaces; i++)
		{
			if (mesh->mFaces[i].mNumIndices != 3)
			{
				LOG("WARNING, geometry face with != 3 indices!");
				ret = false;
				break;
			}
			else
				memcpy(&new_mesh.indices[i * 3], mesh->mFaces[i].mIndices, sizeof(unsigned int) * 3);
		}

		//Load indicies to VRAM
		glGenBuffers(1, &new_mesh.indices_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_mesh.indices_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(new_mesh.indices), new_mesh.indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else
		LOG("Mesh has no faces");

	return ret;
}
