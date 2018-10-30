#include "GameObject.h"
#include "Application.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

// Creation of Root Node from a file
GameObject::GameObject(const char* path, const aiScene* scene, const aiNode * root_obj, const char * file_path)
{
	this->name = "Scene";
	this->root = this;

	this->children.push_back(App->importer->ImportGameObject(path, scene, root_obj, this));

	if (this->parent == nullptr)
		this->camera = new ComponentCamera(this);

	if (this->parent == nullptr)
		this->transform = new ComponentTransform();

	Start();
}

void GameObject::Start()
{
	if (camera != nullptr)
		camera->Start();
}

void GameObject::Update(float dt)
{
	root->CalculateGlobalTransforms();

	if (camera != nullptr)
		camera->Update(dt);

	Draw();

}

void GameObject::Draw()
{
	for (int i = 0; i < this->meshes.size(); i++)
	{
		glPushMatrix();
		glMultMatrixf(this->transform->global_transform.Transposed().ptr());
		this->meshes[i]->Draw();
		glPopMatrix();
	}

	if (this->BoundingBox != nullptr && (App->renderer3D->bounding_box || this->active))
	{
		if (this->transform->to_update)
		{
			SetTransformedBoundBox();
			//this->transform->to_update = false;
		}
		this->DrawBB();
	}

	for (int i = 0; i < this->children.size(); i++)
		this->children[i]->Draw();
		
	if (camera != nullptr)
		camera->Draw();
}

void GameObject::DrawBB()
{
	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);

	glColor3f(0.f, 1.f, 0.f);

	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->maxPoint.z);
	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->maxPoint.z);

	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->maxPoint.z);
	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->maxPoint.z);

	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->maxPoint.z);
	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->minPoint.z);

	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->minPoint.z);
	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->minPoint.z);

	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->minPoint.z);
	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->maxPoint.z);

	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->minPoint.z);
	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->minPoint.z);

	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->minPoint.z);
	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->minPoint.z);

	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->minPoint.z);
	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->maxPoint.z);

	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->minPoint.z);
	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->minPoint.z);

	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->maxPoint.z);
	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->maxPoint.z);

	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->maxPoint.z);
	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->maxPoint.z);

	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->maxPoint.z);
	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->minPoint.z);

	glColor3f(0, 1, 0);

	glEnd();

	if (App->renderer3D->lighting)
		glEnable(GL_LIGHTING);
}

vec GameObject::getObjectCenter()
{
	if (this->BoundingBody == nullptr)
		this->SetBoundBox();

	float aux_x = (this->BoundingBox->maxPoint.x + this->BoundingBox->minPoint.x) / 2;
	float aux_y = (this->BoundingBox->maxPoint.y + this->BoundingBox->minPoint.y) / 2;
	float aux_z = (this->BoundingBox->maxPoint.z + this->BoundingBox->minPoint.z) / 2;

	return vec(aux_x, aux_y, aux_z);
}

float GameObject::SetBoundBox()
{
	max_distance_point = 0;

	this->BoundingBox = new AABB(vec(INT_MAX, INT_MAX, INT_MAX),vec(INT_MIN, INT_MIN, INT_MIN));

	if (this->children.size() == 0)
	{
		SetBoundBoxFromMeshes();
	}
	else
	{
		// Set Children Bounding Boxes
		for (int i = 0; i < this->children.size(); i++)
			this->children[i]->SetBoundBox();

		for (int i = 0; i < this->children.size(); i++)
		{

			// Setting the BB min and max points

			if (this->BoundingBox->maxPoint.x < this->children[i]->BoundingBox->maxPoint.x)
				this->BoundingBox->maxPoint.x = this->children[i]->BoundingBox->maxPoint.x;																	
			if (this->BoundingBox->minPoint.x > this->children[i]->BoundingBox->minPoint.x)
				this->BoundingBox->minPoint.x = this->children[i]->BoundingBox->minPoint.x;																	
			if (this->BoundingBox->maxPoint.y < this->children[i]->BoundingBox->maxPoint.y)
				this->BoundingBox->maxPoint.y = this->children[i]->BoundingBox->maxPoint.y;																	
			if (this->BoundingBox->minPoint.y > this->children[i]->BoundingBox->minPoint.y)
				this->BoundingBox->minPoint.y = this->children[i]->BoundingBox->minPoint.y;
			if (this->BoundingBox->maxPoint.z < this->children[i]->BoundingBox->maxPoint.z)
				this->BoundingBox->maxPoint.z = this->children[i]->BoundingBox->maxPoint.z;												 						
			if (this->BoundingBox->minPoint.z > this->children[i]->BoundingBox->minPoint.z)
				this->BoundingBox->minPoint.z = this->children[i]->BoundingBox->minPoint.z;

		}
	}

	// Set Return Value
	{
		if (abs(this->BoundingBox->maxPoint.x) > max_distance_point) { max_distance_point = abs(this->BoundingBox->maxPoint.x); }
		if (abs(this->BoundingBox->maxPoint.y) > max_distance_point) { max_distance_point = abs(this->BoundingBox->maxPoint.y); }
		if (abs(this->BoundingBox->maxPoint.z) > max_distance_point) { max_distance_point = abs(this->BoundingBox->maxPoint.z); }
		if (abs(this->BoundingBox->minPoint.x) > max_distance_point) { max_distance_point = abs(this->BoundingBox->minPoint.x); }
		if (abs(this->BoundingBox->minPoint.y) > max_distance_point) { max_distance_point = abs(this->BoundingBox->minPoint.y); }
		if (abs(this->BoundingBox->minPoint.z) > max_distance_point) { max_distance_point = abs(this->BoundingBox->minPoint.z); }
	}



	return max_distance_point;
}

void GameObject::SetTransformedBoundBox()
{
	max_distance_point = 0;

	this->BoundingBox = new AABB(vec(INT_MAX, INT_MAX, INT_MAX), vec(INT_MIN, INT_MIN, INT_MIN));

	if (this->children.size() == 0)
	{
		if (this->meshes.size() > 0)
		{
			for (int i = 0; i < this->meshes.size(); i++)
			{
				math::AABB auxBB = *this->meshes[i]->BoundingBox;
				auxBB.TransformAsAABB(this->meshes[i]->parent->transform->global_transform);

				// Setting the BB min and max points

				if (this->BoundingBox->maxPoint.x < auxBB.maxPoint.x)
					this->BoundingBox->maxPoint.x = auxBB.maxPoint.x;
				if (this->BoundingBox->minPoint.x > auxBB.minPoint.x)
					this->BoundingBox->minPoint.x = auxBB.minPoint.x;
				if (this->BoundingBox->maxPoint.y < auxBB.maxPoint.y)
					this->BoundingBox->maxPoint.y = auxBB.maxPoint.y;
				if (this->BoundingBox->minPoint.y > auxBB.minPoint.y)
					this->BoundingBox->minPoint.y = auxBB.minPoint.y;
				if (this->BoundingBox->maxPoint.z < auxBB.maxPoint.z)
					this->BoundingBox->maxPoint.z = auxBB.maxPoint.z;
				if (this->BoundingBox->minPoint.z > auxBB.minPoint.z)
					this->BoundingBox->minPoint.z = auxBB.minPoint.z;
			}
		}
	}
	else
	{
		// Set Children Bounding Boxes
		for (int i = 0; i < this->children.size(); i++)
			this->children[i]->SetBoundBox();

		for (int i = 0; i < this->children.size(); i++)
		{
			math::AABB auxBB = *this->children[i]->BoundingBox;
			auxBB.TransformAsAABB(this->children[i]->transform->global_transform);

			// Setting the BB min and max points with transforms

			if (this->BoundingBox->maxPoint.x < auxBB.maxPoint.x)
				this->BoundingBox->maxPoint.x = auxBB.maxPoint.x;
			if (this->BoundingBox->minPoint.x > auxBB.minPoint.x)
				this->BoundingBox->minPoint.x = auxBB.minPoint.x;
			if (this->BoundingBox->maxPoint.y < auxBB.maxPoint.y)
				this->BoundingBox->maxPoint.y = auxBB.maxPoint.y;
			if (this->BoundingBox->minPoint.y > auxBB.minPoint.y)
				this->BoundingBox->minPoint.y = auxBB.minPoint.y;
			if (this->BoundingBox->maxPoint.z < auxBB.maxPoint.z)
				this->BoundingBox->maxPoint.z = auxBB.maxPoint.z;
			if (this->BoundingBox->minPoint.z > auxBB.minPoint.z)
				this->BoundingBox->minPoint.z = auxBB.minPoint.z;

		}
	}

	// Set Return Value
	{
		if (abs(this->BoundingBox->maxPoint.x) > max_distance_point) { max_distance_point = abs(this->BoundingBox->maxPoint.x); }
		if (abs(this->BoundingBox->maxPoint.y) > max_distance_point) { max_distance_point = abs(this->BoundingBox->maxPoint.y); }
		if (abs(this->BoundingBox->maxPoint.z) > max_distance_point) { max_distance_point = abs(this->BoundingBox->maxPoint.z); }
		if (abs(this->BoundingBox->minPoint.x) > max_distance_point) { max_distance_point = abs(this->BoundingBox->minPoint.x); }
		if (abs(this->BoundingBox->minPoint.y) > max_distance_point) { max_distance_point = abs(this->BoundingBox->minPoint.y); }
		if (abs(this->BoundingBox->minPoint.z) > max_distance_point) { max_distance_point = abs(this->BoundingBox->minPoint.z); }
	}
}

void GameObject::SetBoundBoxFromMeshes()
{
	this->BoundingBox = new AABB(vec(INT_MAX, INT_MAX, INT_MAX), vec(INT_MIN, INT_MIN, INT_MIN));

	if (this->meshes.size() > 0)
	{
		for (int i = 0; i < this->meshes.size(); i++)
		{
			// Setting the BB min and max points

			if (this->BoundingBox->maxPoint.x < this->meshes[i]->BoundingBox->maxPoint.x)
				this->BoundingBox->maxPoint.x = this->meshes[i]->BoundingBox->maxPoint.x;
			if (this->BoundingBox->minPoint.x > this->meshes[i]->BoundingBox->minPoint.x)
				this->BoundingBox->minPoint.x = this->meshes[i]->BoundingBox->minPoint.x;
			if (this->BoundingBox->maxPoint.y < this->meshes[i]->BoundingBox->maxPoint.y)
				this->BoundingBox->maxPoint.y = this->meshes[i]->BoundingBox->maxPoint.y;
			if (this->BoundingBox->minPoint.y > this->meshes[i]->BoundingBox->minPoint.y)
				this->BoundingBox->minPoint.y = this->meshes[i]->BoundingBox->minPoint.y;
			if (this->BoundingBox->maxPoint.z < this->meshes[i]->BoundingBox->maxPoint.z)
				this->BoundingBox->maxPoint.z = this->meshes[i]->BoundingBox->maxPoint.z;
			if (this->BoundingBox->minPoint.z > this->meshes[i]->BoundingBox->minPoint.z)
				this->BoundingBox->minPoint.z = this->meshes[i]->BoundingBox->minPoint.z;
		}
	}
}

void GameObject::AdjustObjects()
{
	int i = 0;
	for (i; i < this->children.size(); i++)
	{
		if (this->children[i]->to_pop == true)
		{
			delete this->children[i];
			this->children[i] = nullptr;
			break;
		}
	}

	for (int j = i; j < this->children.size() - 1; j++)
	{
		this->children[j] = this->children[j + 1];
	}

	this->children.pop_back();
}

void GameObject::AdjustMaterials()
{
	int i = 0;
	for (i; i < this->materials.size(); i++)
	{
		if (this->materials[i]->to_pop == true)
		{
			delete this->materials[i];
			this->materials[i] = nullptr;
			break;
		}
	}

	for (int j = i; j < this->materials.size() - 1; j++)
	{
		this->materials[j] = this->materials[j + 1];
	}

	this->materials.pop_back();
}

void GameObject::AdjustMeshes()
{
	int i = 0;
	for (i; i < this->meshes.size(); i++)
	{
		if (this->meshes[i]->to_pop == true)
		{
			delete this->meshes[i];
			this->meshes[i] = nullptr;
			break;
		}
	}

	for (int j = i; j < this->meshes.size() - 1; j++)
	{
		this->meshes[j] = this->meshes[j + 1];
	}

	this->meshes.pop_back();
}

void GameObject::CalculateGlobalTransforms()
{
	if (this->parent != nullptr)
		this->transform->global_transform = this->parent->transform->global_transform * this->transform->local_transform;

	else
		transform->global_transform = transform->local_transform;
	
	if (this->children.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = this->children.begin(); it != this->children.end(); it++)
		{
			(*it)->CalculateGlobalTransforms();
		}
	}
}

void GameObject::CleanUp()
{
	

	for (int i = 0; i < this->meshes.size(); i++)
	{
		this->meshes[i]->CleanUp();
		delete this->meshes[i];
		this->meshes[i] = nullptr;
	}
	this->meshes.clear();

	for (int i = 0; i < this->materials.size(); i++)
	{
		this->materials[i]->CleanUp();
		delete this->materials[i];
		this->materials[i] = nullptr;
	}
	this->meshes.clear();

	delete this->transform;
	this->transform = nullptr;

	if (this->BoundingBox != nullptr) {
		delete this->BoundingBox;
		this->BoundingBox = nullptr;
	}

	if (this->BoundingBody != nullptr) {
		this->BoundingBody->DelMathBody();
		delete this->BoundingBody;
		this->BoundingBody = nullptr;
	}

	for (int i = 0; i < this->children.size(); i++)
		this->children[i]->CleanUp();

	this->parent = nullptr;
	this->root = nullptr;
	this->name.clear();
}

ComponentTransform * GameObject::GetParentTransform()
{
	if (parent != nullptr)
		return parent->transform;
	else
		return nullptr;
}
