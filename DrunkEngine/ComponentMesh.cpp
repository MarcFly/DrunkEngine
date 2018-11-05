#include "ComponentMesh.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "Inspector.h"
#include "GameObject.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "MeshImport.h"

ComponentMesh::ComponentMesh()
{
	SetBaseVals();
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
	if (this->num_vertex > 0)
	{
		glGenBuffers(1, &this->id_vertex);
		glBindBuffer(GL_ARRAY_BUFFER, this->id_vertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->num_vertex * 3, this->vertex, GL_STATIC_DRAW);
	}

	// **Unbind Buffer**
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Index Buffer
	if (this->num_index > 0)
	{
	glGenBuffers(1, &this->id_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * this->num_index, this->index, GL_STATIC_DRAW);
	}
	// **Unbind Buffer**
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// Texture Coordinates / UVs Buffer
	if (this->num_uvs > 0)
	{
		glGenBuffers(1, &this->id_uvs);
		glBindBuffer(GL_ARRAY_BUFFER, this->id_uvs);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->num_uvs * 3, this->tex_coords, GL_STATIC_DRAW);
	}
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
	glPushMatrix();
	glMultMatrixf(this->parent->GetTransform()->global_transform.Transposed().ptr());

	if (App->scene->active_cameras.size() > 0 && isMeshInsideFrustum(App->scene->active_cameras[0], this->parent->BoundingBox))
	{
		if (index != nullptr && vertex != nullptr)
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
	}

	glPopMatrix();
}

void ComponentMesh::DrawMesh()
{

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, this->id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id_index);

	if (this->Material_Ind != -1 && tex_coords != nullptr)
	{
		std::vector<Component*> cmp_mats;
		cmp_mats = parent->GetComponents(CT_Material);
		ComponentMaterial* mat = nullptr;

		for (int i = 0; i < cmp_mats.size(); i++)
		{
			mat = cmp_mats[i]->AsMaterial();
			if (mat != nullptr && mat->count_number == this->Material_Ind)
				break;
		}

		if (mat != nullptr)
		{
			Color c = mat->default_print;
			glColor4f(c.r, c.g, c.b, c.a);

			// Technically this will do for all textures in a material, so for diffuse, ambient,... 
			// I don't know if the texture coordinates should be binded every time for each texture or just binding different textures
			if (mat->textures.size() > 0)
			{
				for (int i = 0; i < mat->textures.size(); i++)
				{
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glBindBuffer(GL_ARRAY_BUFFER, this->id_uvs);
					glTexCoordPointer(3, GL_FLOAT, 0, NULL);

					glBindTexture(GL_TEXTURE_2D, mat->textures[i]->id_tex);
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

bool ComponentMesh::isMeshInsideFrustum(const ComponentCamera * cam, const AABB * bounding_box)
{
	float3 vCorner[8];
	int iTotalIn = 0;
	Plane planes[6];
	cam->frustum.GetPlanes(planes);

	bounding_box->GetCornerPoints(vCorner); // get the corners of the box into the vCorner array
	// test all 8 corners against the 6 sides
	// if all points are behind 1 specific plane, we are out
	// if we are in with all points, then we are fully in
	for (int p = 0; p < 6; ++p) {
		int iInCount = 8;
		int iPtIn = 1;
		for (int i = 0; i < 8; ++i) {
			// test this point against the planes
			if (!planes[p].AreOnSameSide(vCorner[i], cam->frustum.CenterPoint())) {
				iPtIn = 0;
				--iInCount;
			}
		}
		// were all the points outside of plane p?
		if (iInCount == 0)
			return false;
		// check if they were all on the right side of the plane
		iTotalIn += iPtIn;
	}
	// so if iTotalIn is 6, then all are inside the view
	if (iTotalIn == 6)
		return true;
	// we must be partly in then otherwise
	return true;
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

	this->parent = nullptr;
	this->root = nullptr;
}

void ComponentMesh::Load(JSON_Object* comp)
{
	this->name = json_object_get_string(comp, "mesh_name");
	App->importer->mesh_i->ImportMesh(name.c_str(), this);
}

void ComponentMesh::Save(JSON_Array* comps)
{
	JSON_Value* append = json_value_init_object();
	JSON_Object* curr = json_value_get_object(append);

	json_object_dotset_number(curr, "properties.type", type);

	json_object_dotset_string(curr, "properties.mesh_name", name.c_str());

	json_array_append_value(comps, append);

	App->importer->mesh_i->ExportMesh(this);
}
