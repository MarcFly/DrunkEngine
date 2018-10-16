#include "ComponentTransform.h"
#include "Assimp/include/scene.h"

ComponentTransform::ComponentTransform(const aiMatrix4x4 * t, GameObject* par)
{
	//aiQuaternion rotation_quat; = rotation_quat.GetEuler();
	t->Decompose(this->transform_scale, this->transform_rotate, this->transform_position);

	this->parent = par;
}

ComponentTransform::ComponentTransform(const aiMatrix4x4 * t, ComponentMesh* par)
{
	//aiQuaternion rotation_quat; = rotation_quat.GetEuler();
	t->Decompose(this->transform_scale, this->transform_rotate, this->transform_position);

	this->mparent = par;
}

void ComponentTransform::CleanUp()
{
	this->parent = nullptr;
}
