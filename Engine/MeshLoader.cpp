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
	bool equal_size_floats = (sizeof(float) == sizeof(GLfloat)) ? true : false;
	bool equal_size_uints = (sizeof(unsigned int) == sizeof(GLuint)) ? true : false;

	GLuint num_vertices = mesh->mNumVertices;

	//Load Vertices
	if (mesh->HasPositions())
	{
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
	}
	else
	{
		LOG("Mesh has no vertices");
		ret = false;
	}

	// Copy faces
	if (mesh->HasFaces() && ret == true)
	{
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
	}
	else
		LOG("Mesh has no faces");

	if (mesh->HasTextureCoords(0) && ret == true)
	{
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
						memcpy(uvs[i], mesh->mTextureCoords[i], sizeof(GLfloat) * num_uv_components[i]);
				}
			}
			else
			{
				LOG("Size of float not equal to GLfloat\nTransforming all values to OpenGL standard (GLfloat)\nThis might take a while");

				for (int j = 0; j < num_vertices; j++)
				{
					//Slowest thing ever but better than nothing if sizes do not mach (may be a better way)
					uvs[i][j * num_uv_components[i]] = mesh->mTextureCoords[i][j].x;
					if(num_uv_components[i] >= 2)
						uvs[i][j * num_uv_components[i] + 1] = mesh->mTextureCoords[i][j].y;
					if (num_uv_components[i] >= 3)
						uvs[i][j * num_uv_components[i] + 2] = mesh->mTextureCoords[i][j].z;
				}
			}

			glGenBuffers(1, &uv_ids[i]);
			glBindBuffer(GL_ARRAY_BUFFER, uv_ids[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * num_vertices * num_uv_components[i], &uvs[i], GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		new_mesh.SetUVs(num_uv_channels, num_uv_components, uv_ids, uvs);
	}
	else
		LOG("Mesh has no texture coordinates in channel 0");

	//Normals will either exist or be generated here all to 1.0f, 1.0f, 1.0f
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

	if (mesh->HasVertexColors(0) && ret == true)
	{
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
	}
	else
		LOG("Mesh has no vertex colors");
	
	return ret;
}
