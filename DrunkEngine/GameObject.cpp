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

		this->transform = new ComponentTransform(&obj->mTransformation, this);
		
	}

}
