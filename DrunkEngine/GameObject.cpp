#include "GameObject.h"
#include "Application.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

GameObject::GameObject(const aiScene* scene, const aiNode* obj_root, GameObject* par)
{
	this->parent = par;
	this->root = this->parent;
	while (this->root->parent != nullptr)
		this->root = this->root->parent;

	this->name = obj_root->mName.C_Str();
	CreateThisObj(scene, obj_root);
}

GameObject::GameObject(const aiScene* scene, const aiNode * root_obj, const char * file_path)
{
	this->name = file_path;
	this->root = this;
	CreateThisObj(scene, root_obj);
}

void GameObject::CreateThisObj(const aiScene* scene, const aiNode * obj)
{
	float vertex_aux = 0;

	if (this->parent == nullptr)
		this->camera = new ComponentCamera(this);

	for (int i = 0; i < obj->mNumMeshes; i++)
		this->meshes.push_back(new ComponentMesh(scene->mMeshes[obj->mMeshes[i]], this));

	for (int i = 0; i < scene->mNumMaterials; i++)
		this->materials.push_back(new ComponentMaterial(scene->mMaterials[i], this));

	for (int i = 0; i < obj->mNumChildren; i++)
		this->children.push_back(new GameObject(scene, obj->mChildren[i], this));

	this->transform = new ComponentTransform(&obj->mTransformation, this);

	SetBoundBox();

	Start();
}

void GameObject::Start()
{
	if (camera != nullptr)
		camera->Start();
}

void GameObject::Update(float dt)
{
	if (camera != nullptr)
		camera->Update(dt);

	Draw();
}

void GameObject::Draw()
{
	for (int i = 0; i < this->meshes.size(); i++)
		this->meshes[i]->Draw();

	if ((App->renderer3D->bounding_box || this->active) && this->BoundingBox != nullptr)
		this->DrawBB();

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
	float aux_x = (this->BoundingBox->maxPoint.x + this->BoundingBox->minPoint.x) / 2;
	float aux_y = (this->BoundingBox->maxPoint.y + this->BoundingBox->minPoint.y) / 2;
	float aux_z = (this->BoundingBox->maxPoint.z + this->BoundingBox->minPoint.z) / 2;

	return vec(aux_x, aux_y, aux_z);
}

float GameObject::SetBoundBox()
{
	float ret = 0;

	this->BoundingBox = new AABB(vec(INT_MAX, INT_MAX, INT_MAX),vec(INT_MIN, INT_MIN, INT_MIN));

	if (this->children.size() == 0)
	{
		SetBoundBoxFromMeshes();
	}
	else
	{
		// Set Children Bounding Boxes if they don't have one
		for (int i = 0; i < this->children.size(); i++)
			if(this->children[i]->BoundingBox == nullptr)
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
		if (abs(this->BoundingBox->maxPoint.x) > ret) { ret = abs(this->BoundingBox->maxPoint.x); }
		if (abs(this->BoundingBox->maxPoint.y) > ret) { ret = abs(this->BoundingBox->maxPoint.y); }
		if (abs(this->BoundingBox->maxPoint.z) > ret) { ret = abs(this->BoundingBox->maxPoint.z); }
		if (abs(this->BoundingBox->minPoint.x) > ret) { ret = abs(this->BoundingBox->minPoint.x); }
		if (abs(this->BoundingBox->minPoint.y) > ret) { ret = abs(this->BoundingBox->minPoint.y); }
		if (abs(this->BoundingBox->minPoint.z) > ret) { ret = abs(this->BoundingBox->minPoint.z); }
	}

	return ret;
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
