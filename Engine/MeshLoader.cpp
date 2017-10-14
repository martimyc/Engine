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
				App->scene_manager->game_object->AddComponent(new_m);
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
	if (mesh->mNumVertices > 0)
	{
		GLuint num_vertices = 0;
		GLuint vertex_id = 0;
		GLfloat* vertices = nullptr;

		num_vertices = mesh->mNumVertices;
		glGenBuffers(1, &vertex_id);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_id);

		vertices = new GLfloat[num_vertices * 3];

		if (sizeof(float) != sizeof(GLfloat))
		{
			LOG("Size of float not equal to GLfloat\nTransforming all values to OpenGL standard (GLfloat)\nThis might take a while");

			for (uint i = 0; i < num_vertices; i++)
			{
				//Slowest thing ever but better than nothing if sizes do not mach (may be a better way)
				vertices[i * 3] = mesh->mVertices[i][0];
				vertices[i - 3 + 1] = mesh->mVertices[i][1];
				vertices[i * 3 + 2] = mesh->mVertices[i][2];
			}
		}
		else
			memcpy(vertices, mesh->mVertices, sizeof(GLfloat) * num_vertices * 3);

		LOG("New mesh with %d vertices", num_vertices);

		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* num_vertices * 3, vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		new_mesh.SetVertices(vertex_id, num_vertices, vertices);
	}
	else
		LOG("Mesh has no vertices");

	// Copy faces
	if (mesh->HasFaces())
	{
		bool uints_equal_size = true;

		GLuint indices_id = 0;
		GLuint num_indices = 0;
		GLuint* indices = nullptr;

		num_indices = mesh->mNumFaces * 3;
		indices = new GLuint[num_indices]; // assume each face is a triangle

		if (sizeof(unsigned int) != sizeof(GLuint))
		{
			LOG("Size of unsigned int not equal to GLuint\nTransforming all values to OpenGL standard (GLuint)\nThis might take a while");
			uints_equal_size = false;
		}

		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			if (mesh->mFaces[i].mNumIndices != 3)
			{
				LOG("WARNING, geometry face with != 3 indices!");
				ret = false;
				break;
			}
			else
				if (uints_equal_size)
					memcpy(&indices[i * 3], mesh->mFaces[i].mIndices, sizeof(GLuint) * 3);
				else
					for (uint j = 0; j < 3; j++)
					{
						//Slowest thing ever but better than nothing if sizes do not mach (may be a better way)
						indices[i * 3 + j] = mesh->mFaces[i].mIndices[j];
					}
		}

		//Load indicies to VRAM
		glGenBuffers(1, &indices_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * num_indices, indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		new_mesh.SetIndices(indices_id, num_indices, indices);
	}
	else
		LOG("Mesh has no faces");

	uint uv_chanels = mesh->GetNumUVChannels();
	if (uv_chanels > 1)
	{
		LOG("Mesh has %i UV channels", uv_chanels);

		for (int i = 0; i < uv_chanels; i++)
		{

		}
	}
	else
		LOG("Mesh has no UV");


	if (mesh->HasNormals())
	{

	}
	else
		LOG("Mesh has no Normals");

	
	return ret;
}
