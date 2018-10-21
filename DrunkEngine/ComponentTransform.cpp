#include "ComponentTransform.h"
#include "Assimp/include/scene.h"

ComponentTransform::ComponentTransform(const aiMatrix4x4 * t, GameObject* par)
{
	//aiQuaternion rotation_quat; = rotation_quat.GetEuler();
	aiQuaternion rot;
	aiVector3D pos;
	aiVector3D scale;

	t->Decompose(scale, rot, pos);
	this->transform_scale = float3(scale.x, scale.y, scale.z);
	this->transform_rotate_quat = Quat(rot.x, rot.y, rot.z, rot.w);
	this->transform_position = float3(pos.x, pos.y, pos.z);

	SetTransformRotation(transform_rotate_quat);

	this->parent = par;
}

ComponentTransform::ComponentTransform(const aiMatrix4x4 * t, ComponentMesh* par)
{

	aiQuaternion rot;
	aiVector3D pos;
	aiVector3D scale;

	t->Decompose(scale, rot, pos);
	this->transform_scale = float3(scale.x, scale.y, scale.z);
	this->transform_rotate_quat = Quat(rot.x, rot.y, rot.z, rot.w);
	this->transform_position = float3(pos.x, pos.y, pos.z);

	this->mparent = par;
}

void ComponentTransform::SetTransformPosition(const int pos_x, const int pos_y, const int pos_z)
{
	transform_position.x = pos_x;
	transform_position.y = pos_y;
	transform_position.z = pos_z;

}

void ComponentTransform::SetTransformRotation(const Quat rot_quat)
{
	transform_rotate_quat = rot_quat;
	transform_rotate_euler = RadToDeg(transform_rotate_quat.ToEulerXYZ());
}

void ComponentTransform::SetTransformRotation(const float3 rot_vec)
{
	transform_rotate_quat = Quat::FromEulerXYZ(DegToRad(rot_vec.x), DegToRad(rot_vec.y), DegToRad(rot_vec.z));
	transform_rotate_euler = rot_vec;
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