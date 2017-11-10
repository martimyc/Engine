#include "Globals.h"
#include "UID.h"
#include "FileSystem.h"
#include "Application.h"
#include "Mesh.h"
#include "MeshImporter.h"

MeshImporter::MeshImporter()
{}

MeshImporter::~MeshImporter()
{}

unsigned int MeshImporter::GetTotalSize(const aiMesh * mesh) const
{
	unsigned int total_size = FORMAT_SIZE;
	total_size += sizeof(GLuint);

	if (mesh->HasPositions())
		total_size += sizeof(GLfloat) * mesh->mNumVertices * 3;

	if (mesh->HasFaces())
	{
		total_size += sizeof(GLuint);
		total_size += sizeof(GLuint) * 3 * mesh->mNumFaces;
	}

	total_size += sizeof(GLuint);
	if (mesh->HasTextureCoords(0))
	{
		for (int i = 0; i < mesh->GetNumUVChannels(); i++)
		{
			total_size += sizeof(GLuint);
			total_size += sizeof(GLfloat) * mesh->mNumVertices * mesh->mNumUVComponents[i];
		}
	}

	total_size += sizeof(GLfloat) * mesh->mNumVertices * 3;

	if (mesh->HasVertexColors(0))
	{
		total_size += sizeof(GLuint);
		for (int i = 0; i < mesh->GetNumColorChannels(); i++)
			total_size += sizeof(GLfloat) * mesh->mNumVertices * 4;
	}

	return total_size;
}

const UID MeshImporter::Import(const aiMesh * mesh, const std::string & scene_path, const std::string & name)
{
	bool ret = true;

	char format[FORMAT_SIZE] = FORMAT_MESH;
	char* buffer = new char[GetTotalSize(mesh)];
	char* iterator = buffer;

	//First specify format
	memcpy(iterator, format, FORMAT_SIZE);
	iterator += FORMAT_SIZE;

	memcpy(iterator, name.c_str(), name.length());
	iterator += name.length();

	GLuint num_vertices = mesh->mNumVertices;
	memcpy(iterator, &num_vertices, sizeof(GLuint));
	iterator += sizeof(GLuint);

	//Load Vertices
	if (mesh->HasPositions())
	{
		memcpy(iterator, mesh->mVertices, sizeof(GLfloat) * num_vertices * 3);
		iterator += sizeof(GLfloat) * num_vertices * 3;
	}
	else
	{
		LOG("Mesh has no vertices");
		ret = false;
	}

	// Copy faces
	if (mesh->HasFaces() && ret == true)
	{
		GLuint num_faces = mesh->mNumFaces;
		memcpy(iterator, &num_faces, sizeof(GLuint));
		iterator += sizeof(GLuint);

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
				memcpy(iterator, mesh->mFaces[i].mIndices, sizeof(GLuint) * 3);
				iterator += sizeof(GLuint) * 3;
			}
		}
	}
	else
	{
		LOG("Mesh has no faces");
		ret = false;
	}

	GLuint num_uv_channels = mesh->GetNumUVChannels();
	memcpy(iterator, &num_uv_channels, sizeof(GLuint));
	iterator += sizeof(GLuint);

	if (mesh->HasTextureCoords(0) && ret == true)
	{
		GLuint num_uv_components = 0;

		for (int i = 0; i < num_uv_channels; i++)
		{
			num_uv_components = mesh->mNumUVComponents[i];
			memcpy(iterator, &num_uv_components, sizeof(GLuint));
			iterator += sizeof(GLuint);

			if (num_uv_components == 3)
			{
				memcpy(iterator, mesh->mTextureCoords[i], sizeof(GLfloat) * num_vertices * num_uv_components);
				iterator += sizeof(GLfloat) * num_vertices * num_uv_components;
			}
			else
			{
				//Slowest but better than nothing saving lots of zeroes in vram (z and maybe y coordinates), slow load but less vram usage
				for (int j = 0; j < num_vertices; j++)
				{
					memcpy(iterator, &mesh->mTextureCoords[i][j].x, sizeof(GLfloat) * num_uv_components);
					iterator += sizeof(GLfloat) * num_uv_components;
				}
			}
		}
	}
	else
		LOG("Mesh has no texture coordinates in channel 0");

	if (ret == true)
	{
		//Normals will either exist or be generated here all to 1.0f, 1.0f, 1.0f
		if (mesh->HasNormals() && ret == true)
			memcpy(iterator, mesh->mNormals, sizeof(GLfloat) * num_vertices * 3);
		else
		{
			LOG("Mesh has no Normals even thow assimp should have generated them, all normals will be set to: 1.0f, 1.0f, 1.0f");
			memset(iterator, 1.0f, sizeof(GLfloat) * num_vertices * 3);
		}
		iterator += sizeof(GLfloat) * num_vertices * 3;
	}

	int size_now = iterator - buffer;
	int test = GetTotalSize(mesh);

	if (mesh->HasVertexColors(0) && ret == true)
	{
		GLuint num_color_channels = mesh->GetNumColorChannels();
		memcpy(iterator, &num_color_channels, sizeof(GLuint));
		iterator += sizeof(GLuint);

		for (int i = 0; i < num_color_channels; i++)
		{
			memcpy(iterator, mesh->mColors[i], sizeof(GLfloat) * num_vertices * 4);
			iterator += sizeof(GLfloat) * num_vertices * 4;	
		}
	}
	else
		LOG("Mesh has no vertex colors");

	// Bones and TangentsAndBitangents not loaded yet TODO

	uint length = iterator - buffer;

	UID id(buffer, length);

	if (App->file_system->SaveFile(buffer, length, LIBRARY_MESHES_FOLDER, id.uid, "mm") == false)
	{
		LOG("Could not save file %s correctlly", name.c_str());
		return UID();
	}

	delete[] buffer;

	return id;
}

MeshSource* MeshImporter::Load(const UID & id, const MeshLoadConfiguration& config)
{
	//TODO
	return nullptr;
	/*Mesh* new_mesh = nullptr;
	char* buffer = nullptr;
	char* iterator = nullptr;
	uint length = 0;

	std::string path(App->file_system->GetMeshes());
	path += "\\";
	path += id.uid;
	path += ".mm";
	length = App->file_system->LoadFileBinary(path, &buffer);

	if (buffer != nullptr && length != 0)
	{
		iterator = buffer;
		iterator += FORMAT_SIZE;

		std::string name(iterator);
		iterator += name.length();

		new_mesh = new Mesh(name);

		GLuint num_vertices;
		memcpy(&num_vertices, iterator, sizeof(GLuint));
		iterator += sizeof(GLuint);

		if (num_vertices > 0)
		{
			GLuint vertex_id = 0;
			GLfloat* vertices = new GLfloat[num_vertices * 3];

			memcpy(vertices, iterator, sizeof(GLfloat) * num_vertices * 3);
			iterator += sizeof(GLfloat) * num_vertices * 3;

			LOG("New mesh with %d vertices", num_vertices);

			glGenBuffers(1, &vertex_id);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_id);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* num_vertices * 3, vertices, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			new_mesh->SetVertices(vertex_id, num_vertices, vertices);
		}
		else
		{
			LOG("Mesh has no vertices");
			return nullptr;
		}

		GLuint num_faces;
		memcpy(&num_faces, iterator, sizeof(GLuint));
		iterator += sizeof(GLuint);

		if (num_faces > 0)
		{
			GLuint indices_id = 0;
			GLuint num_indices = num_faces * 3;
			GLuint* indices = new GLuint[num_indices];

			memcpy(indices, iterator, sizeof(GLuint) * num_indices);
			iterator += sizeof(GLuint) * num_indices;

			//Load indicies to VRAM
			glGenBuffers(1, &indices_id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * num_indices, indices, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			new_mesh->SetIndices(indices_id, num_indices, indices);
		}
		else
		{
			LOG("Mesh has no faces");
			return nullptr;
		}

		GLuint num_uv_channels;
		memcpy(&num_uv_channels, iterator, sizeof(GLuint));
		iterator += sizeof(GLuint);

		if (num_uv_channels > 0)
		{
			GLuint* uv_ids = new GLuint[num_uv_channels];
			GLuint* num_uv_components = new GLuint[num_uv_channels];
			GLfloat** uvs = new GLfloat*[num_uv_channels];

			for (int i = 0; i < num_uv_channels; i++)
			{
				memcpy(&num_uv_components[i], iterator, sizeof(GLuint));
				iterator += sizeof(GLuint);
				uvs[i] = new GLfloat[num_vertices * num_uv_components[i]];

				memcpy(uvs[i], iterator, sizeof(GLfloat) * num_uv_components[i] * num_vertices);
				iterator += sizeof(GLfloat) * num_uv_components[i] * num_vertices;

				glGenBuffers(1, &uv_ids[i]);
				glBindBuffer(GL_ARRAY_BUFFER, uv_ids[i]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * num_vertices * num_uv_components[i], uvs[i], GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			new_mesh->SetUVs(num_uv_channels, num_uv_components, uv_ids, uvs);
		}
		else
			LOG("Mesh has no UVs");

		GLuint normals_id = 0;
		GLfloat* normals = new GLfloat[num_vertices * 3];

		memcpy(normals, iterator, sizeof(GLfloat) * num_vertices * 3);
		iterator += sizeof(GLfloat) * num_vertices * 3;

		glGenBuffers(1, &normals_id);
		glBindBuffer(GL_ARRAY_BUFFER, normals_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * num_vertices * 3, normals, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		new_mesh->SetNormals(normals_id, normals);

		GLuint num_color_channels;
		memcpy(&num_color_channels, iterator, sizeof(GLuint));
		iterator += sizeof(GLuint);

		if (num_color_channels < 0)
		{
			GLuint* color_ids = new GLuint[num_color_channels];
			GLfloat** colors = new GLfloat*[num_color_channels];

			for (int i = 0; i < num_color_channels; i++)
			{
				colors[i] = new GLfloat[num_vertices * 4];

				memcpy(colors[i], iterator, sizeof(GLfloat) * num_vertices * 4);
				iterator += sizeof(GLfloat) * num_vertices * 4;

				glGenBuffers(1, &color_ids[i]);
				glBindBuffer(GL_ARRAY_BUFFER, color_ids[i]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * num_vertices * 4, colors[i], GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			new_mesh->SetColors(num_color_channels, color_ids, colors);
		}
		else
			LOG("Mesh has no vertex colors");
	}
	return new_mesh;*/
}


