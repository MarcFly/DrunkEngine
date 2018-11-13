#include "ResourceMesh.h"
#include "Application.h"

void MetaMesh::LoadMetaFile(const char* file)
{
	App->importer->mesh_i->LoadMeta(file, this);
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
