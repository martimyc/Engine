#include "Application.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "TextureManager.h"
#include "SceneLoader.h"

SceneLoader::SceneLoader(const char * name, bool start_enabled) : Module(name, start_enabled)
{}

SceneLoader::~SceneLoader()
{}

bool SceneLoader::Init()
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool SceneLoader::CleanUp()
{
	// detach log stream
	aiDetachAllLogStreams();
	return true;
}

bool SceneLoader::LoadScene(const std::string& path) const
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
		if (scene->HasMaterials())
		{
			//Load all materials first so that we can bind them to a mesh later when we load them
			App->scene_manager->ReserveMaterialSpace(scene->mNumMaterials);

			for (unsigned int i = 0; i < scene->mNumMaterials; i++)
			{
				LOG("Loading Material %i", i);

				Material* new_material = new Material();
				ret = LoadMaterial(scene->mMaterials[i], *new_material, dir);

				if (ret == false)
					LOG("Material (%i) did't load correctly", i);
				else
					App->scene_manager->AddMaterial(new_material);
			}
		}
		else
			LOG("Scene has no materials");

		if (scene->HasMeshes())
		{
			
			// Load all meshes (for now into the same game object)
			App->scene_manager->game_object->ReserveComponentSpace(scene->mNumMeshes);

			for (unsigned int i = 0; i < scene->mNumMeshes; i++)
			{
				LOG("Loading Mesh %i", i);

				Mesh* new_mesh = new Mesh();
				ret = LoadMesh(scene->mMeshes[i], *new_mesh);

				if (ret == false)
					LOG("Mesh (%i) did't load correctly", i);
				else
					App->scene_manager->game_object->AddComponent(new_mesh);
			}
		}
		else
			LOG("More than a single mesh in scene, will Import all as one Game Object");

		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", path);

	return ret;
}

bool SceneLoader::LoadMesh(const aiMesh * mesh, Mesh& new_mesh) const
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

	if(App->scene_manager->HasMaterials())
		new_mesh.SetMaterial(mesh->mMaterialIndex);

	// Bones and TangentsAndBitangents not loaded yet TODO
	
	return ret;
}

bool SceneLoader::LoadVertices(const aiMesh* mesh, const GLuint & num_vertices, Mesh & new_mesh, bool equal_size_floats, bool equal_size_uints) const
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

bool SceneLoader::LoadIndices(const aiMesh* mesh, const GLuint & num_vertices, Mesh & new_mesh, bool equal_size_floats, bool equal_size_uints) const
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

bool SceneLoader::LoadTextureCoordinates(const aiMesh* mesh, const GLuint & num_vertices, Mesh & new_mesh, bool equal_size_floats, bool equal_size_uints) const
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

bool SceneLoader::LoadNormals(const aiMesh* mesh, const GLuint & num_vertices, Mesh & new_mesh, bool equal_size_floats, bool equal_size_uints) const
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

bool SceneLoader::LoadColors(const aiMesh* mesh, const GLuint & num_vertices, Mesh & new_mesh, bool equal_size_floats, bool equal_size_uints) const
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

bool SceneLoader::LoadMaterial(const aiMaterial* material, Material& new_material, const std::string& dir) const
{
	//We will assume all textures are 2D for now

	bool ret = true;

	if (material->GetTextureCount(aiTextureType_NONE))
	{
		LOG("Found are invalid textures");
		ret = false;
	}

	GLuint num_difusse_textures = material->GetTextureCount(aiTextureType_DIFFUSE);
	
	//TODO load the rest of texture types
	/*GLuint num_specular_textures = material->GetTextureCount(aiTextureType_SPECULAR);
	GLuint num_ambient_textures = material->GetTextureCount(aiTextureType_AMBIENT);
	GLuint num_emissive_textures = material->GetTextureCount(aiTextureType_EMISSIVE);
	GLuint num_height_textures = material->GetTextureCount(aiTextureType_HEIGHT);
	GLuint num_normals_textures = material->GetTextureCount(aiTextureType_NORMALS);
	GLuint num_shininess_textures = material->GetTextureCount(aiTextureType_SHININESS);
	GLuint num_opacity_textures = material->GetTextureCount(aiTextureType_OPACITY);
	GLuint num_displacement_textures = material->GetTextureCount(aiTextureType_DISPLACEMENT);
	GLuint num_lightmap_textures = material->GetTextureCount(aiTextureType_LIGHTMAP);
	GLuint num_reflection_textures = material->GetTextureCount(aiTextureType_REFLECTION);
	GLuint num_unknown_textures = material->GetTextureCount(aiTextureType_UNKNOWN);*/
	
	if (num_difusse_textures > 0)
	{
		for (int i = 0; i < num_difusse_textures; i++)
		{
			aiString Path;

			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string full_path = dir + Path.data;
				
				Texture* new_texture = new Texture(full_path, TT_DIFFUSE, GL_TEXTURE_2D, 0);

				if (!App->texture_manager->LoadTexture(full_path, *new_texture))
				{
					delete new_texture;
					LOG("Error loading texture '%s'\n", full_path.c_str());
					ret = false;
				}
				else
					new_material.AddTexture(new_texture);
			}
		}
	}
	else
		LOG("No difusse textures in this material");

	//TODO Blend and strenght functions when lightning is done

	return ret;
}
