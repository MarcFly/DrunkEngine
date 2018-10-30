#include "ComponentTransform.h"
#include "GameObject.h"
#include "Assimp/include/scene.h"
#include "GameObject.h"

ComponentTransform::ComponentTransform(const aiMatrix4x4 * t, GameObject* par)
{
	SetBaseVals();
	SetFromMatrix(t);
	parent = par;

	SetLocalTransform();
}

void ComponentTransform::SetFromMatrix(const aiMatrix4x4* t)
{
	aiVector3D local_scale;
	aiVector3D pos;
	aiQuaternion rot;

	t->Decompose(local_scale, rot, pos);

	scale = float3(local_scale.x, local_scale.y, local_scale.z);
	rotate_quat = Quat(rot.x, rot.y, rot.z, rot.w);
	position = float3(pos.x, pos.y, pos.z);

	SetTransformRotation(rotate_quat);
}

void ComponentTransform::SetTransformPosition(const float pos_x, const float pos_y, const float pos_z)
{
	position.x = pos_x;
	position.y = pos_y;
	position.z = pos_z;

	SetLocalTransform();
}

void ComponentTransform::SetTransformRotation(const Quat rot_quat)
{
	rotate_quat = rot_quat;
	rotate_euler = RadToDeg(rotate_quat.ToEulerXYZ());
	SetLocalTransform();
}

void ComponentTransform::SetTransformRotation(const float3 rot_vec)
{
	rotate_quat = Quat::FromEulerXYZ(DegToRad(rot_vec.x), DegToRad(rot_vec.y), DegToRad(rot_vec.z));
	rotate_euler = rot_vec;
	SetLocalTransform();
}

void ComponentTransform::SetTransformScale(const float scale_x, const float scale_y, const float scale_z)
{	
	scale.x = scale_x;
	scale.y = scale_y;
	scale.z = scale_z;
	SetLocalTransform();
}

void ComponentTransform::SetLocalTransform()
{
	float4x4 local_pos = float4x4::FromTRS(position, Quat::identity, float3::one);
	float4x4 local_scale = float4x4::FromTRS(float3::zero, Quat::identity, scale);

	local_transform = local_pos * (float4x4)rotate_quat * local_scale;

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
	parent = nullptr;
}

void ComponentTransform::Load(JSON_Object* comp)
{
	position.x = json_object_dotget_number(comp, "position.x");
	position.y = json_object_dotget_number(comp, "position.y");
	position.z = json_object_dotget_number(comp, "position.z");
	
	scale.x = json_object_dotget_number(comp, "scale.x");
	scale.y = json_object_dotget_number(comp, "scale.y");
	scale.z = json_object_dotget_number(comp, "scale.z");

	rotate_quat.w = json_object_dotget_number(comp, "rotate_quat.w");
	rotate_quat.x = json_object_dotget_number(comp, "rotate_quat.x");
	rotate_quat.y = json_object_dotget_number(comp, "rotate_quat.y");
	rotate_quat.z = json_object_dotget_number(comp, "rotate_quat.z");

	SetTransformRotation(rotate_quat);
}

void ComponentTransform::Save(JSON_Array* comps)
{
	JSON_Value* append = json_value_init_object();
	JSON_Object* curr = json_value_get_object(append);

	json_object_dotset_number(curr, "properties.type", type);

	json_object_dotset_number(curr, "properties.position.x", position.x);
	json_object_dotset_number(curr, "properties.position.y", position.y);
	json_object_dotset_number(curr, "properties.position.z", position.z);

	json_object_dotset_number(curr, "properties.scale.x", scale.x);
	json_object_dotset_number(curr, "properties.scale.y", scale.y);
	json_object_dotset_number(curr, "properties.scale.z", scale.z);

	json_object_dotset_number(curr, "properties.rotate_quat.w", rotate_quat.w);
	json_object_dotset_number(curr, "properties.rotate_quat.x", rotate_quat.x);
	json_object_dotset_number(curr, "properties.rotate_quat.y", rotate_quat.y);
	json_object_dotset_number(curr, "properties.rotate_quat.z", rotate_quat.z);

	json_array_append_value(comps, append);
}