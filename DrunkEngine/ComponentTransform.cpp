#include "ComponentTransform.h"
#include "Assimp/include/scene.h"

ComponentTransform::ComponentTransform(const aiMatrix4x4 * t, GameObject* par) : parent{ par }
{
	aiQuaternion rot;
	aiVector3D pos;
	aiVector3D local_scale;

	t->Decompose(local_scale, rot, pos);
	this->scale = float3(scale.x, scale.y, scale.z);
	this->rotate_quat = Quat(rot.x, rot.y, rot.z, rot.w);
	this->position = float3(pos.x, pos.y, pos.z);

	SetTransformRotation(rotate_quat);
}

ComponentTransform::ComponentTransform(const aiMatrix4x4 * t, ComponentMesh* par) : mparent{par}
{

	aiQuaternion rot;
	aiVector3D pos;
	aiVector3D local_scale;

	t->Decompose(local_scale, rot, pos);
	this->scale = float3(local_scale.x, local_scale.y, local_scale.z);
	this->rotate_quat = Quat(rot.x, rot.y, rot.z, rot.w);
	this->position = float3(pos.x, pos.y, pos.z);
}

void ComponentTransform::SetTransformPosition(const int pos_x, const int pos_y, const int pos_z)
{
	position.x = pos_x;
	position.y = pos_y;
	position.z = pos_z;

}

void ComponentTransform::SetTransformRotation(const Quat rot_quat)
{
	rotate_quat = rot_quat;
	rotate_euler = RadToDeg(rotate_quat.ToEulerXYZ());
}

void ComponentTransform::SetTransformRotation(const float3 rot_vec)
{
	rotate_quat = Quat::FromEulerXYZ(DegToRad(rot_vec.x), DegToRad(rot_vec.y), DegToRad(rot_vec.z));
	rotate_euler = rot_vec;
}

void ComponentTransform::SetTransformScale(const int scale_x, const int scale_y, const int scale_z)
{	
	scale.x = scale_x;
	scale.y = scale_y;
	scale.z = scale_z;
}

void ComponentTransform::CleanUp()
{
	this->parent = nullptr;
}