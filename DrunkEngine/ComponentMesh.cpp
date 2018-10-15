#include "ComponentMesh.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "GeoPropertiesWindow.h"

ComponentMesh::ComponentMesh(const aiMesh * mesh, GameObject * par)
{
	this->parent = par;
	this->root = parent->root;

	this->num_vertex = mesh->mNumVertices;
	this->vertex = new float[this->num_vertex * 3];
	this->num_faces = mesh->mNumFaces;

	this->name = mesh->mName.C_Str();

	memcpy(this->vertex, mesh->mVertices, 3 * sizeof(float)*this->num_vertex);

	if (mesh->HasFaces())
	{
		this->num_index = mesh->mNumFaces * 3;
		this->index = new GLuint[this->num_index];

		this->num_normal = this->num_index * 2;
		this->normal = new float[this->num_normal];

		for (uint j = 0; j < mesh->mNumFaces; j++)
		{
			if (mesh->mFaces[j].mNumIndices != 3)
				App->ui->console_win->AddLog("WARNING, geometry face with != 3 indices!");
			else
			{
				memcpy(&this->index[j * 3], mesh->mFaces[j].mIndices, 3 * sizeof(GLuint));

				SetNormals(j);
			}
		}

		SetMeshBoundBox();
	}

	SetTexCoords(mesh);

	this->Material_Ind = mesh->mMaterialIndex;

	GenBuffers();

	App->ui->console_win->AddLog("New mesh with %d vertices, %d indices, %d faces (tris)", this->num_vertex, this->num_index, this->num_faces);

	App->ui->geo_properties_win->CheckMeshInfo();


}

bool ComponentMesh::SetTexCoords(const aiMesh * mesh)
{
	bool ret = true;

	// Set TexCoordinates
	if (mesh->HasTextureCoords(0))
	{
		this->num_uvs = this->num_vertex;
		this->tex_coords = new float[this->num_uvs * 3];
		memcpy(this->tex_coords, mesh->mTextureCoords[0], this->num_uvs * sizeof(float) * 3);
	}
	else
		App->ui->console_win->AddLog("No texture coordinates to be set");

	return ret;
}

void ComponentMesh::SetNormals(const int& index)
{
	float aux[9];

	aux[0] = this->vertex[this->index[index * 3] * 3];
	aux[1] = this->vertex[(this->index[index * 3] * 3) + 1];
	aux[2] = this->vertex[(this->index[index * 3] * 3) + 2];
	aux[3] = this->vertex[(this->index[(index * 3) + 1] * 3)];
	aux[4] = this->vertex[(this->index[(index * 3) + 1] * 3) + 1];
	aux[5] = this->vertex[(this->index[(index * 3) + 1] * 3) + 2];
	aux[6] = this->vertex[(this->index[(index * 3) + 2] * 3)];
	aux[7] = this->vertex[(this->index[(index * 3) + 2] * 3) + 1];
	aux[8] = this->vertex[(this->index[(index * 3) + 2] * 3) + 2];

	float p1 = (aux[0] + aux[3] + aux[6]) / 3;
	float p2 = (aux[1] + aux[4] + aux[7]) / 3;
	float p3 = (aux[2] + aux[5] + aux[8]) / 3;

	this->normal[index * 6] = p1;
	this->normal[index * 6 + 1] = p2;
	this->normal[index * 6 + 2] = p3;

	vec v1(aux[0], aux[1], aux[2]);
	vec v2(aux[3], aux[4], aux[5]);
	vec v3(aux[6], aux[7], aux[8]);

	vec norm = (v2 - v1).Cross(v3 - v1);
	norm.Normalize();

	this->normal[index * 6 + 3] = p1 + norm.x;
	this->normal[index * 6 + 4] = p2 + norm.y;
	this->normal[index * 6 + 5] = p3 + norm.z;
}

void ComponentMesh::GenBuffers()
{
	// Vertex Buffer
	glGenBuffers(1, &this->id_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, this->id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->num_vertex * 3, this->vertex, GL_STATIC_DRAW);

	// **Unbind Buffer**
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Index Buffer
	glGenBuffers(1, &this->id_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * this->num_index, this->index, GL_STATIC_DRAW);

	// **Unbind Buffer**
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// Texture Coordinates / UVs Buffer
	glGenBuffers(1, (GLuint*) &(this->id_uvs));
	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)this->id_uvs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * this->num_uvs * 3, this->tex_coords, GL_STATIC_DRAW);

	// **Unbind Buffer**
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ComponentMesh::SetMeshBoundBox()
{
	float max_x = INT_MIN, max_y = INT_MIN, max_z = INT_MIN, min_x = INT_MAX, min_y = INT_MAX, min_z = INT_MAX;

	for (int i = 0; i < this->num_vertex; i++)
	{
		if (max_x < this->vertex[i * 3])
			max_x = this->vertex[i * 3];
		if (min_x > this->vertex[i * 3])
			min_x = this->vertex[i * 3];
		if (max_y < this->vertex[i * 3 + 1])
			max_y = this->vertex[i * 3 + 1];
		if (min_y > this->vertex[i * 3 + 1])
			min_y = this->vertex[i * 3 + 1];
		if (max_z < this->vertex[i * 3 + 2])
			max_z = this->vertex[i * 3 + 2];
		if (min_z > this->vertex[i * 3 + 2])
			min_z = this->vertex[i * 3 + 2];
	}

	this->BoundingBox.maxPoint = vec(max_x, max_y, max_z);
	this->BoundingBox.minPoint = vec(min_x, min_y, min_z);
}
