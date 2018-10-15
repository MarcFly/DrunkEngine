#include "GameObject.h"
#include "Application.h"

GameObject::GameObject(const aiScene* scene, const aiNode* obj_root, GameObject* par)
{
	this->parent = par;
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

	for (int i = 0; i < obj->mNumChildren; i++)
	{
		for (int j = 0; i < obj->mChildren[i]->mNumChildren; j++)
			this->children.push_back(new GameObject(scene, obj->mChildren[i], this));

		for (int j = 0; j < obj->mNumMeshes; j++)
			this->meshes.push_back(new ComponentMesh(scene->mMeshes[obj->mMeshes[j]], this));

		for (int j = 0; j < scene->mNumMaterials; j++)
			this->materials.push_back(new ComponentMaterial(scene->mMaterials[j], this));

		this->transform = new ComponentTransform(obj->mTransformation, this);
		
		App->camera->SetToObj(this, SetBoundBox());
	}

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

	if (children.size() < 0)
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
				this->BoundingBox->minPoint.y = this->children[i]->BoundingBox->minPoint.y)												 						
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
				this->BoundingBox->minPoint.y = this->meshes[i]->BoundingBox->minPoint.y)
				if (this->BoundingBox->maxPoint.z < this->children[i]->BoundingBox->maxPoint.z)
					this->BoundingBox->maxPoint.z = this->children[i]->BoundingBox->maxPoint.z;
			if (this->BoundingBox->minPoint.z > this->children[i]->BoundingBox->minPoint.z)
				this->BoundingBox->minPoint.z = this->children[i]->BoundingBox->minPoint.z;
		}
	}
}
