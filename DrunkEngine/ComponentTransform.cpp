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

void ComponentTransform::Load(JSON_Value* scene, const char* file)
{

}

void ComponentTransform::Save(JSON_Value* scene, const char* file)
{
	JSON_Object* curr = json_value_get_object(scene);

	std::string obj = std::to_string(this->parent->UUID) + ".";
	std::string set_val;

	set_val = obj + "type";
	json_object_dotset_number(curr, set_val.c_str(), type);

	set_val = obj + "position.x";
	json_object_dotset_number(curr, set_val.c_str(), position.x);
	set_val = obj + "position.y";
	json_object_dotset_number(curr, set_val.c_str(), position.y);
	set_val = obj + "position.z";
	json_object_dotset_number(curr, set_val.c_str(), position.z);

	set_val = obj + "scale.x";
	json_object_dotset_number(curr, set_val.c_str(), scale.x);
	set_val = obj + "scale.y";
	json_object_dotset_number(curr, set_val.c_str(), scale.y);
	set_val = obj + "scale.z";
	json_object_dotset_number(curr, set_val.c_str(), scale.z);

	set_val = obj + "rotate_quat.w";
	json_object_dotset_number(curr, set_val.c_str(), rotate_quat.w);
	set_val = obj + "rotate_quat.x";
	json_object_dotset_number(curr, set_val.c_str(), rotate_quat.x);
	set_val = obj + "rotate_quat.y";
	json_object_dotset_number(curr, set_val.c_str(), rotate_quat.y);
	set_val = obj + "rotate_quat.z";
	json_object_dotset_number(curr, set_val.c_str(), rotate_quat.z);

	json_serialize_to_file(scene, file);
	scene = json_parse_file(file);
}