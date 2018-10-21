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

void ComponentTransform::SetTransformPosition(const int pos_x, const int pos_y, const int pos_z)
{
	transform_position.x = pos_x;
	transform_position.y = pos_y;
	transform_position.z = pos_z;

}

void ComponentTransform::SetTransformRotation(const int rot_x, const int rot_y, const int rot_z)
{
	Quat rot_quat = Quat::FromEulerXYZ(DegToRad(rot_x), DegToRad(rot_y), DegToRad(rot_z));
	
	transform_rotate = { rot_quat.w, rot_quat.x, rot_quat.y, rot_quat.z };
}

void ComponentTransform::SetTransformScale(const int scale_x, const int scale_y, const int scale_z)
{	
	transform_scale.x = scale_x;
	transform_scale.y = scale_y;
	transform_scale.z = scale_z;
}

void ComponentTransform::CleanUp()
{
	this->parent = nullptr;
}