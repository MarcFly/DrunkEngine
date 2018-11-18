#include "ResourceMesh.h"
#include "Application.h"

ResourceMesh::~ResourceMesh()
{
	UnloadMem();
}

void ResourceMesh::GenBuffers()
{
	// Vertex Buffer
	ResourceMesh* r_mesh = this;

	if (r_mesh->num_vertex > 0)
	{
		glGenBuffers(1, &r_mesh->id_vertex);
		glBindBuffer(GL_ARRAY_BUFFER, r_mesh->id_vertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * r_mesh->num_vertex * 3, r_mesh->vertex, GL_STATIC_DRAW);
	}

	// **Unbind Buffer**
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Index Buffer
	if (r_mesh->num_index > 0)
	{
		glGenBuffers(1, &r_mesh->id_index);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r_mesh->id_index);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * r_mesh->num_index, r_mesh->index, GL_STATIC_DRAW);
	}
	// **Unbind Buffer**
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// Texture Coordinates / UVs Buffer
	if (r_mesh->num_uvs > 0)
	{
		glGenBuffers(1, &r_mesh->id_uvs);
		glBindBuffer(GL_ARRAY_BUFFER, r_mesh->id_uvs);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * r_mesh->num_uvs * 3, r_mesh->tex_coords, GL_STATIC_DRAW);
	}
	// **Unbind Buffer**
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ResourceMesh::UnloadMem()
{
	if (index != nullptr)
	{
		glDeleteBuffers(1, &id_index);
		delete[] index;
		index = nullptr;
	}

	if (tex_coords)
	{
		glDeleteBuffers(1, &id_uvs);
		delete[] tex_coords;
		tex_coords = nullptr;
	}

	if (vertex != nullptr)
	{
		glDeleteBuffers(1, &id_vertex);
		delete[] vertex;
		vertex = nullptr;
	}

	if (normal != nullptr)
	{
		// Have to load normals to GPU as it will drop a lot the performance in CPU mode
		delete[] normal;
		normal = nullptr;
	}
}

//--------------------------------------------------------------------------------------------------------------

void MetaMesh::LoadMetaFile(const char* file)
{
	App->importer->mesh_i->LoadMeta(file, this);
}