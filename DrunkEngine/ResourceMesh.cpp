#include "ResourceMesh.h"
#include "Application.h"

ResourceMesh::~ResourceMesh()
{
	UnloadMem();
}

ResourceMesh::ResourceMesh(const ResourceMesh * cpy)
{

	num_index = cpy->num_index;
	num_vertex = cpy->num_vertex;
	num_uvs = cpy->num_uvs;
	num_normal = cpy->num_normal;
	num_faces = cpy->num_faces;
	mat = cpy->mat;
	def_color = cpy->def_color;

	index = new uint[num_index];
	vertex = new float[num_vertex * 3];
	tex_coords = new float[num_uvs * 3];
	normal = new float[num_normal * 3];
	vert_normals = new float[num_vertex * 3];

	SetValsFromMesh(cpy);

	GenBuffers();
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

	if (r_mesh->num_vertex > 0)
	{
		glGenBuffers(1, &r_mesh->id_vert_normals);
		glBindBuffer(GL_ARRAY_BUFFER, r_mesh->id_vert_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * r_mesh->num_vertex * 3, r_mesh->vert_normals, GL_STATIC_DRAW);
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

void ResourceMesh::SetValsFromMesh(const ResourceMesh * cpy)
{
	memcpy(index, cpy->index, sizeof(GLuint)*num_index);
	memcpy(vertex, cpy->vertex, sizeof(GLfloat)*num_vertex * 3);
	memcpy(tex_coords, cpy->tex_coords, sizeof(GLfloat)*num_uvs * 3);
	memcpy(normal, cpy->normal, sizeof(GLfloat)*num_normal*3);
	memcpy(vert_normals, cpy->vert_normals, sizeof(GLfloat)*num_vertex * 3);
}

void ResourceMesh::UnloadBuffers()
{
	if (index != nullptr)
		glDeleteBuffers(1, &id_index);

	if (tex_coords)
		glDeleteBuffers(1, &id_uvs);

	if (vertex != nullptr)
		glDeleteBuffers(1, &id_vertex);

	/*if (normal != nullptr)
		glDeleteBuffers(1, &id_normal);
		*/

	if (vert_normals != nullptr)
		glDeleteBuffers(1, &id_vert_normals);
}

void ResourceMesh::UnloadMem()
{
	if (index != nullptr)
	{
		delete[] index;
		index = nullptr;
	}

	if (tex_coords)
	{
		delete[] tex_coords;
		tex_coords = nullptr;
	}

	if (vertex != nullptr)
	{
		delete[] vertex;
		vertex = nullptr;
	}

	if (normal != nullptr)
	{
		// Have to load normals to GPU as it will drop a lot the performance in CPU mode
		delete[] normal;
		normal = nullptr;
	}

	if (vert_normals != nullptr)
	{
		delete[] vert_normals;
		vert_normals = nullptr;
	}

	UnloadBuffers();
}

//--------------------------------------------------------------------------------------------------------------

void MetaMesh::LoadMetaFile(const char* file)
{
	App->importer->mesh_i->LoadMeta(file, this);
}