#include "ComponentTransform.h"
#include "Gameobject.h"
#include "Assimp/include/scene.h"

ComponentTransform::ComponentTransform()
{
	this->transform_scale = float3::one;
	this->transform_rotate_quat = Quat::identity;
	this->transform_position = float3::zero;

	SetTransformRotation(transform_rotate_quat);

	this->parent = nullptr;

	SetLocalTransform();
}

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

	SetLocalTransform();
}

void ComponentTransform::SetTransformPosition(const float pos_x, const float pos_y, const float pos_z)
{
	transform_position.x = pos_x;
	transform_position.y = pos_y;
	transform_position.z = pos_z;
	SetLocalTransform();
}

void ComponentTransform::SetTransformRotation(const Quat rot_quat)
{
	transform_rotate_quat = rot_quat;
	transform_rotate_euler = RadToDeg(transform_rotate_quat.ToEulerXYZ());
	SetLocalTransform();
}

void ComponentTransform::SetTransformRotation(const float3 rot_vec)
{
	transform_rotate_quat = Quat::FromEulerXYZ(DegToRad(rot_vec.x), DegToRad(rot_vec.y), DegToRad(rot_vec.z));
	transform_rotate_euler = rot_vec;
	SetLocalTransform();
}

void ComponentTransform::SetTransformScale(const float scale_x, const float scale_y, const float scale_z)
{	
	transform_scale.x = scale_x;
	transform_scale.y = scale_y;
	transform_scale.z = scale_z;
	SetLocalTransform();
}

void ComponentTransform::SetLocalTransform()
{
	float4x4 local_pos = float4x4::FromTRS(transform_position, Quat::identity, float3::one);
	float4x4 local_scale = float4x4::FromTRS(float3::zero, Quat::identity, transform_scale);

	local_transform = local_pos * (float4x4)transform_rotate_quat * local_scale;

	to_update = true;
}

void ComponentTransform::RecursiveSetToUpdate(ComponentTransform * t)
{
	t->to_update = true;

	if (t->parent != nullptr && t->parent->GetParentTransform() != nullptr)
		RecursiveSetToUpdate(t->parent->GetParentTransform());
}

void ComponentTransform::CleanUp()
{
	this->parent = nullptr;
}