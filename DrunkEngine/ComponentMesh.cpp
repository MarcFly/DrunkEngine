#include "ComponentMesh.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "GeoPropertiesWindow.h"
#include "GameObject.h"
#include "ComponentMaterial.h"

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

	//if (this->BoundingBox != nullptr)
	//	delete this->BoundingBox;

	this->BoundingBox = new AABB(vec(min_x, min_y, min_z), vec(max_x, max_y, max_z));
}

void ComponentMesh::Draw()
{
	if (App->renderer3D->faces)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		DrawMesh();
	}

	if (App->renderer3D->wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		DrawMeshWire();
	}

	// Set Default Color back
	Color def = App->camera->background;
	glColor4f(def.r, def.g, def.b, def.a);

	if (App->renderer3D->render_normals)
		this->DrawNormals();
	
}

void ComponentMesh::DrawMesh()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, this->id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id_index);

	if (this->Material_Ind != -1)
	{
		if (this->Material_Ind < this->parent->materials.size())
		{
			Color c = this->parent->materials[Material_Ind]->default_print;
			glColor4f(c.r, c.g, c.b, c.a);

			// Technically this will do for all textures in a material, so for diffuse, ambient,... 
			// I don't know if the texture coordinates should be binded every time for each texture or just binding different textures
			if (this->parent->materials[Material_Ind]->textures.size() > 0)
			{
				for (int i = 0; i < this->parent->materials[Material_Ind]->textures.size(); i++)
				{
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glBindBuffer(GL_ARRAY_BUFFER, this->id_uvs);
					glTexCoordPointer(3, GL_FLOAT, 0, NULL);

					glBindTexture(GL_TEXTURE_2D, this->parent->materials[Material_Ind]->textures[i]->id_tex);
				}
			}
		}
		else
		{
			this->Material_Ind = -1;
			App->ui->console_win->AddLog("Tried to render non-existing Material!");
		}
	}
	else
	{
		glColor4f(1, 1, 1, 1);
	}

	// Draw
	glDrawElements(GL_TRIANGLES, this->num_index, GL_UNSIGNED_INT, NULL);

	// Unbind Buffers
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ComponentMesh::DrawMeshWire()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, this->id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id_index);

	glColor3f(1, 1, 1);

	// Draw
	glDrawElements(GL_TRIANGLES, this->num_index, GL_UNSIGNED_INT, NULL);

	// Unbind Buffers
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ComponentMesh::DrawNormals()
{
	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f);

	for (int k = 0; k < this->num_normal / 6; k++)
	{
		glVertex3f(this->normal[k * 6], this->normal[k * 6 + 1], this->normal[k * 6 + 2]);

		vec norm(this->normal[k * 6 + 3] - this->normal[k * 6], this->normal[k * 6 + 4] - this->normal[k * 6 + 1], this->normal[k * 6 + 5] - this->normal[k * 6 + 2]);
		norm = norm.Mul(App->renderer3D->normal_length);

		glVertex3f(this->normal[k * 6] + norm.x, this->normal[k * 6 + 1] + norm.y, this->normal[k * 6 + 2] + norm.z);
	}
	glColor3f(0, 1, 0);
	glEnd();

	
}

void ComponentMesh::CleanUp()
{
	glDeleteBuffers(1, &this->id_index);
	glDeleteBuffers(1, &this->id_uvs);
	glDeleteBuffers(1, &this->id_vertex);

	delete this->index; this->index = nullptr;
	delete this->normal; this->normal = nullptr;
	delete this->tex_coords; this->tex_coords = nullptr;
	delete this->vertex; this->vertex = nullptr;

	if (this->BoundingBox != nullptr) {
		delete this->BoundingBox;
		this->BoundingBox = nullptr;
	}

	if (this->BoundingBody != nullptr) {
		this->BoundingBody->DelMathBody();
		delete this->BoundingBody;
		this->BoundingBody = nullptr;
	}

	this->parent = nullptr;
	this->root = nullptr;
}
