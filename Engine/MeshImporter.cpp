#include "Globals.h"
#include "UID.h"
#include "FileSystem.h"
#include "Application.h"
#include "Mesh.h"
#include "KDTreeTriangle.h"
#include "MeshAsset.h"
#include "MeshImporter.h"

MeshImporter::MeshImporter()
{}

MeshImporter::~MeshImporter()
{}

unsigned int MeshImporter::GetTotalSize(const aiMesh * mesh, bool kdt, const KDTreeTriangle& tree) const
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

	total_size += sizeof(bool);

	if (mesh->HasVertexColors(0))
	{
		total_size += sizeof(GLuint);
		for (int i = 0; i < mesh->GetNumColorChannels(); i++)
			total_size += sizeof(GLfloat) * mesh->mNumVertices * 4;
	}

	total_size += sizeof(bool);

	if (kdt)
		total_size += tree.GetSaveSize();

	return total_size;
}

const UID MeshImporter::Import(const aiMesh * mesh, const MeshImportConfiguration* config)
{
	KDTreeTriangle tree;

	//Load Vertices
	unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];

	if (mesh->HasPositions() == false)
	{
		LOG("Mesh has no vertices");
		return false;
	}

	// Copy faces
	if (mesh->HasFaces())
	{
		GLuint num_faces = mesh->mNumFaces;

		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			if (mesh->mFaces[i].mNumIndices != 3)
			{
				LOG("WARNING, geometry face with != 3 indices!");
				return false;
			}
			else
				memcpy(&indices[i * 3], mesh->mFaces[i].mIndices, sizeof(GLuint) * 3);
		}
	}
	else
	{
		LOG("Mesh has no faces");
		return false;
	}

	if (config->kdt)
	{
		math::vec max_vec(mesh->mVertices[0].x, mesh->mVertices[0].y, mesh->mVertices[0].z);
		math::vec min_vec(mesh->mVertices[0].x, mesh->mVertices[0].y, mesh->mVertices[0].z);

		for (int i = 1; i < mesh->mNumVertices; i++)
		{
			if (mesh->mVertices[i].x > max_vec.x)
				max_vec.x = mesh->mVertices[i].x;
			if (mesh->mVertices[i].y > max_vec.y)
				max_vec.y = mesh->mVertices[i].y;
			if (mesh->mVertices[i].z > max_vec.z)
				max_vec.z = mesh->mVertices[i].z;

			if (mesh->mVertices[i].x < min_vec.x)
				min_vec.x = mesh->mVertices[i].x;
			if (mesh->mVertices[i].y < min_vec.y)
				min_vec.y = mesh->mVertices[i].y;
			if (mesh->mVertices[i].z < min_vec.z)
				min_vec.z = mesh->mVertices[i].z;
		}
		tree.AddTriangles( (float*)mesh->mVertices, indices, mesh->mNumFaces * 3, max_vec, min_vec);
	}

	char format[FORMAT_SIZE] = FORMAT_MESH;

	char* buffer = new char[GetTotalSize(mesh, config->kdt, tree)];

	char* iterator = buffer;

	//First specify format
	memcpy(iterator, format, FORMAT_SIZE);
	iterator += FORMAT_SIZE;

	GLuint num_vertices = mesh->mNumVertices;
	memcpy(iterator, &num_vertices, sizeof(GLuint));
	iterator += sizeof(GLuint);

	//Load Vertices
	memcpy(iterator, mesh->mVertices, sizeof(GLfloat) * num_vertices * 3);
	iterator += sizeof(GLfloat) * num_vertices * 3;

	// Copy faces
	GLuint num_indices = mesh->mNumFaces * 3;
	memcpy(iterator, &num_indices, sizeof(GLuint));
	iterator += sizeof(GLuint);

	memcpy(iterator, indices, sizeof(GLuint) *num_indices);
	iterator += sizeof(GLuint) * num_indices;
	delete [] indices;

	//UVs
	GLuint num_uv_channels = mesh->GetNumUVChannels();
	memcpy(iterator, &num_uv_channels, sizeof(GLuint));
	iterator += sizeof(GLuint);

	if (mesh->HasTextureCoords(0))
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

	//Normals will Always exsist thanks to assimp either loading or generating them
	memcpy(iterator, mesh->mNormals, sizeof(GLfloat) * num_vertices * 3);	
	iterator += sizeof(GLfloat) * num_vertices * 3;

	int pos = iterator - buffer;

	if (config->load_colors)
	{
		if (mesh->HasVertexColors(0))
		{
			memcpy(iterator, &config->load_colors, sizeof(bool));
			iterator += sizeof(bool);

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
		{
			LOG("Mesh has no vertex colors");
			bool has_colors = false;
			memcpy(iterator, &has_colors, sizeof(bool));
			iterator += sizeof(bool);
		}
	}
	else
	{
		memcpy(iterator, &config->load_colors, sizeof(bool));
		iterator += sizeof(bool);
	}

	if (config->kdt)
	{
		bool has_kdt = true;
		memcpy(iterator, &has_kdt, sizeof(bool));
		iterator += sizeof(bool);
		tree.Save(&iterator);
	}
	else
	{
		bool kdt = false;
		memcpy(iterator, &kdt, sizeof(bool));
		iterator += sizeof(bool);
	}

	// Bones and TangentsAndBitangents not loaded yet TODO

	uint length = iterator - buffer;

	UID id(buffer, length);

	if (App->file_system->SaveFile(buffer, length, App->file_system->GetMeshes().c_str(), id.GetAsName(), "mm") == false)
	{
		LOG("Could not save mesh correctlly");
		return UID();
	}

	delete[] buffer;

	return id;
}

bool MeshImporter::Load(Mesh* to_load, const MeshLoadConfiguration* config)
{
	char* buffer = nullptr;
	char* iterator = nullptr;
	uint length = 0;

	std::string path(App->file_system->GetMeshes());
	path += "\\";
	path += to_load->GetUID().GetAsName();
	path += ".mm";
	length = App->file_system->LoadFileBinary(path, &buffer);

	if (buffer != nullptr && length != 0)
	{
		iterator = buffer;
		iterator += FORMAT_SIZE;

		MeshSource* new_mesh = new MeshSource();

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
			return false;
		}

		GLuint num_indices;
		memcpy(&num_indices, iterator, sizeof(GLuint));
		iterator += sizeof(GLuint);

		if (num_indices > 0)
		{
			GLuint indices_id = 0;
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
			return false;
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

		bool colors;
		memcpy(&colors, iterator, sizeof(bool));
		iterator += sizeof(bool);

		if (colors)
		{
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

		bool kdt;
		memcpy(&kdt, iterator, sizeof(bool));
		iterator += sizeof(bool);

		if(kdt)
			new_mesh->LoadKDT(&iterator);

		to_load->SetSource(new_mesh);
		return true;
	}
	return false;
}


