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

void ComponentTransform::TransformPosition(const int pos_x, const int pos_y, const int pos_z)
{

}

void ComponentTransform::TransformRotation(const int rot_x, const int rot_y, const int rot_z)
{

}

void ComponentTransform::TransformScale(const int scale_x, const int scale_y, const int scale_z)
{

}

void ComponentTransform::CleanUp()
{
	this->parent = nullptr;
}
