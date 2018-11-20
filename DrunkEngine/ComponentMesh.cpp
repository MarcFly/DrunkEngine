#include "ComponentMesh.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "SceneViewerWindow.h"
#include "GameObject.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "MeshImport.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "FileHelpers.h"

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
		r_mesh->num_uvs = r_mesh->num_vertex;
		r_mesh->tex_coords = new float[r_mesh->num_uvs * 3];
		memcpy(r_mesh->tex_coords, mesh->mTextureCoords[0], r_mesh->num_uvs * sizeof(float) * 3);
	}
	else
		App->ui->console_win->AddLog("No texture coordinates to be set");

	return ret;
}

void ComponentMesh::SetNormals(const int& index)
{
	float aux[9];

	aux[0] = r_mesh->vertex[r_mesh->index[index * 3] * 3];
	aux[1] = r_mesh->vertex[(r_mesh->index[index * 3] * 3) + 1];
	aux[2] = r_mesh->vertex[(r_mesh->index[index * 3] * 3) + 2];
	aux[3] = r_mesh->vertex[(r_mesh->index[(index * 3) + 1] * 3)];
	aux[4] = r_mesh->vertex[(r_mesh->index[(index * 3) + 1] * 3) + 1];
	aux[5] = r_mesh->vertex[(r_mesh->index[(index * 3) + 1] * 3) + 2];
	aux[6] = r_mesh->vertex[(r_mesh->index[(index * 3) + 2] * 3)];
	aux[7] = r_mesh->vertex[(r_mesh->index[(index * 3) + 2] * 3) + 1];
	aux[8] = r_mesh->vertex[(r_mesh->index[(index * 3) + 2] * 3) + 2];

	float p1 = (aux[0] + aux[3] + aux[6]) / 3;
	float p2 = (aux[1] + aux[4] + aux[7]) / 3;
	float p3 = (aux[2] + aux[5] + aux[8]) / 3;

	r_mesh->normal[index * 6] = p1;
	r_mesh->normal[index * 6 + 1] = p2;
	r_mesh->normal[index * 6 + 2] = p3;

	vec v1(aux[0], aux[1], aux[2]);
	vec v2(aux[3], aux[4], aux[5]);
	vec v3(aux[6], aux[7], aux[8]);

	vec norm = (v2 - v1).Cross(v3 - v1);
	norm.Normalize();

	r_mesh->normal[index * 6 + 3] = p1 + norm.x;
	r_mesh->normal[index * 6 + 4] = p2 + norm.y;
	r_mesh->normal[index * 6 + 5] = p3 + norm.z;
}

void ComponentMesh::SetMeshBoundBox()
{
	float max_x = INT_MIN, max_y = INT_MIN, max_z = INT_MIN, min_x = INT_MAX, min_y = INT_MAX, min_z = INT_MAX;

	for (int i = 0; i < r_mesh->num_vertex; i++)
	{
		if (max_x < r_mesh->vertex[i * 3])
			max_x = r_mesh->vertex[i * 3];
		if (min_x > r_mesh->vertex[i * 3])
			min_x = r_mesh->vertex[i * 3];
		if (max_y < r_mesh->vertex[i * 3 + 1])
			max_y = r_mesh->vertex[i * 3 + 1];
		if (min_y > r_mesh->vertex[i * 3 + 1])
			min_y = r_mesh->vertex[i * 3 + 1];
		if (max_z < r_mesh->vertex[i * 3 + 2])
			max_z = r_mesh->vertex[i * 3 + 2];
		if (min_z > r_mesh->vertex[i * 3 + 2])
			min_z = r_mesh->vertex[i * 3 + 2];
	}


	this->BoundingBox = new AABB(vec(min_x, min_y, min_z), vec(max_x, max_y, max_z));
}

void ComponentMesh::Draw()
{
	glPushMatrix();
	glMultMatrixf(this->parent->GetTransform()->global_transform.Transposed().ptr());

	if (CheckMeshValidity())
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

		if (App->renderer3D->render_normals && this->parent->active)
			this->DrawNormals();

		this->parent->static_to_draw = false;

	}
	glPopMatrix();
}

void ComponentMesh::DrawMesh()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, r_mesh->id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r_mesh->id_index);

	if (r_mesh->tex_coords != nullptr)
	{
		LinkMat();

		if (c_mat != nullptr)
		{
			Color c = c_mat->r_mat->default_print;
			glColor4f(c.r, c.g, c.b, c.a);

			// Technically this will do for all textures in a material, so for diffuse, ambient,... 
			// I don't know if the texture coordinates should be binded every time for each texture or just binding different textures
			
			for (int i = 0; i < c_mat->textures.size(); i++)
			{
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glBindBuffer(GL_ARRAY_BUFFER, r_mesh->id_uvs);
				glTexCoordPointer(3, GL_FLOAT, 0, NULL);

				if (c_mat->AlphaTest)
				{
					glEnable(GL_ALPHA_TEST);
					glAlphaFunc(GL_GREATER, c_mat->AlphaVal);
				}

				glBindTexture(GL_TEXTURE_2D, c_mat->textures[i]->id_tex);
				
				glDisable(GL_ALPHA_TEST);
			}	
		}
		else
		{
			if(Material_Ind.CheckValidity())
				App->ui->console_win->AddLog("The material is valid, but not found!");
			else
				App->ui->console_win->AddLog("Tried to render non-existing Material!");
		}


	}
	else
	{
		glColor4f(r_mesh->def_color.r, r_mesh->def_color.g, r_mesh->def_color.b, r_mesh->def_color.a);
	}

	// Draw
	glDrawElements(GL_TRIANGLES, r_mesh->num_index, GL_UNSIGNED_INT, NULL);

	// Unbind Buffers
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ComponentMesh::DrawMeshWire() const
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, r_mesh->id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r_mesh->id_index);

	glColor3f(1, 1, 1);

	// Draw
	glDrawElements(GL_TRIANGLES, r_mesh->num_index, GL_UNSIGNED_INT, NULL);

	// Unbind Buffers
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ComponentMesh::DrawNormals() const
{
	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f);

	for (int k = 0; k < r_mesh->num_normal / 2; k++)
	{
		glVertex3f(r_mesh->normal[k * 6], r_mesh->normal[k * 6 + 1], r_mesh->normal[k * 6 + 2]);

		vec norm(r_mesh->normal[k * 6 + 3] - r_mesh->normal[k * 6], r_mesh->normal[k * 6 + 4] - r_mesh->normal[k * 6 + 1], r_mesh->normal[k * 6 + 5] - r_mesh->normal[k * 6 + 2]);

		norm = norm.Mul(App->renderer3D->normal_length);

		glVertex3f(r_mesh->normal[k * 6] + norm.x, r_mesh->normal[k * 6 + 1] + norm.y, r_mesh->normal[k * 6 + 2] + norm.z);
	}
	glColor3f(0, 1, 0);
	glEnd();

	
}

void ComponentMesh::CleanUp()
{
	App->resources->Unused(UID);
	r_mesh = nullptr;
	c_mat = nullptr;

	if (this->BoundingBox != nullptr) {
		delete this->BoundingBox;
		this->BoundingBox = nullptr;
	}

	this->parent = nullptr;
}

void ComponentMesh::Load(const JSON_Object* comp)
{
	this->name = json_object_get_string(comp, "filename");
	this->UID = DGUID(name.c_str());


	if (!App->resources->InLibrary(UID))
		this->UID = App->resources->AddResource(name.c_str());
	if(App->resources->InLibrary(UID))
		App->importer->mesh_i->LinkMesh(UID, this);

}

void ComponentMesh::Save(JSON_Array* comps)
{
	JSON_Value* append = json_value_init_object();
	JSON_Object* curr = json_value_get_object(append);

	json_object_dotset_number(curr, "properties.type", type);

	json_object_dotset_string(curr, "properties.filename", App->resources->Library.at(UID)->file.c_str());

	json_array_append_value(comps, append);
}

bool ComponentMesh::CheckMeshValidity()
{
	if (App->gameObj->camera_rendering != nullptr && r_mesh != nullptr)
	{
		if (this->parent->is_static == false)
			return App->gameObj->isInsideFrustum(App->gameObj->camera_rendering, this->parent->GetBB());		

		else
		{
			if (App->gameObj->GetSceneKDTree() == nullptr)
				return App->gameObj->isInsideFrustum(App->gameObj->camera_rendering, this->parent->GetBB());
			else
				return this->parent->static_to_draw;
		}
				
	}
	return false;
}

void ComponentMesh::LinkMat()
{
	if(c_mat == nullptr)
		if (Material_Ind.CheckValidity())
		{
			for (int i = 0; i < parent->components.size(); i++)
			{
				if (parent->components[i]->UID == Material_Ind)
				{
					c_mat = parent->components[i]->AsMaterial();
					break;
				}
			}
		}
}