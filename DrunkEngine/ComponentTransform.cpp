#include "ComponentTransform.h"
#include "Assimp/include/scene.h"
#include "GameObject.h"

ComponentTransform::ComponentTransform(const aiMatrix4x4 * t, GameObject* par) : parent{ par }
{
	SetBaseVals();

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

	SetBaseVals();
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

void ComponentTransform::Load(JSON_Array* comps)
{

}

void ComponentTransform::Save(JSON_Array* comps)
{
	JSON_Value* append = json_value_init_object();
	JSON_Object* curr = json_value_get_object(append);

	json_object_dotset_number(curr, "type", type);

	json_object_dotset_number(curr, "position.x", position.x);
	json_object_dotset_number(curr, "position.y", position.y);
	json_object_dotset_number(curr, "position.z", position.z);

	json_object_dotset_number(curr, "scale.x", scale.x);
	json_object_dotset_number(curr, "scale.y", scale.y);
	json_object_dotset_number(curr, "scale.z", scale.z);

	json_object_dotset_number(curr, "rotate_quat.w", rotate_quat.w);
	json_object_dotset_number(curr, "rotate_quat.x", rotate_quat.x);
	json_object_dotset_number(curr, "rotate_quat.y", rotate_quat.y);
	json_object_dotset_number(curr, "rotate_quat.z", rotate_quat.z);

	json_array_append_value(comps, append);
}