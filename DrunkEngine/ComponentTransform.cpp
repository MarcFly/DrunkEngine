#include "ComponentTransform.h"
#include "GameObject.h"
#include "Assimp/include/scene.h"
#include "GameObject.h"
#include "Application.h"

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

void ComponentTransform::SetFromMatrix(const float4x4 * t)
{
	float3 local_scale;
	float3 pos;
	Quat rot;

	t->Decompose(position, rotate_quat, scale);

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
	if (scale.x != scale_x && scale_x > 0.1f)
		scale.x = scale_x;
	if (scale.y != scale_y && scale_y > 0.1f)
		scale.y = scale_y;
	if (scale.z != scale_z && scale_z > 0.1f)
		scale.z = scale_z;

	SetLocalTransform();
}

void ComponentTransform::SetLocalTransform()
{
	local_transform = float4x4::FromTRS(position, rotate_quat, scale);
}

void ComponentTransform::SetWorldPos(const float4x4 new_transform)
{
	aux_world_pos = aux_world_pos * new_transform;
	world_pos = world_pos * new_transform;
}

void ComponentTransform::SetWorldRot(const Quat new_rot)
{
	world_rot = world_rot * world_rot.FromQuat(new_rot, aux_world_pos.Col3(3) - world_pos.Col3(3));

	//world_rot[0][0] = 1.f;
	//world_rot[1][1] = 1.f;
	//world_rot[2][2] = 1.f;
	//world_rot[3][3] = 1.f;

}

void ComponentTransform::CalculateGlobalTransforms()
{
	if (parent->parent != nullptr)
		global_transform = world_pos * world_rot * parent->parent->GetTransform()->global_transform * local_transform;
	else
		global_transform = local_transform;

	global_transform.Decompose(global_pos, global_rot, global_scale);
	//global_pos = global_transform.Col3(3);
	//global_rot = GetRotFromMat(global_transform);
	//global_scale = global_transform.GetScale();

	if (parent->children.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = parent->children.begin(); it != parent->children.end(); it++)
		{
			(*it)->GetTransform()->CalculateGlobalTransforms();
		}
	}

	SetAuxWorldPos();
}

void ComponentTransform::SetAuxWorldPos()
{
	aux_world_pos = float4x4::FromTRS(float3(float3::zero - global_transform.Col3(3)), Quat::identity.Neg(), float3::one);
	aux_world_pos = -aux_world_pos;
}

Quat ComponentTransform::GetRotFromMat(float4x4 mat)
{
	Quat rot;	
	
	rot.w = Sqrt(max(0, 1 + mat.Diagonal3().x + mat.Diagonal3().y + mat.Diagonal3().z)) / 2;
	rot.x = Sqrt(max(0, 1 + mat.Diagonal3().x - mat.Diagonal3().y - mat.Diagonal3().z)) / 2;
	rot.y = Sqrt(max(0, 1 - mat.Diagonal3().x + mat.Diagonal3().y - mat.Diagonal3().z)) / 2;
	rot.z = Sqrt(max(0, 1 - mat.Diagonal3().x - mat.Diagonal3().y + mat.Diagonal3().z)) / 2;

	rot.x *= sgn(rot.x * (mat.Col3(2).y - mat.Col3(1).z));
	rot.y *= sgn(rot.y * (mat.Col3(0).z - mat.Col3(2).x));
	rot.z *= sgn(rot.z * (mat.Col3(1).x - mat.Col3(0).y));

	rot.x = -rot.x;
	rot.y = -rot.y;
	rot.z = -rot.z;

	return rot;
}

void ComponentTransform::CleanUp()
{
	parent = nullptr;
}

void ComponentTransform::Load(const JSON_Object* comp)
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

	SetLocalTransform();

	bool fromAINode = json_object_dotget_boolean(comp, "fromAINode");
	std::string setname;
	if (!fromAINode)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				setname = "world_pos." + std::to_string(j + i * 4);
				world_pos.v[i][j] = json_object_dotget_number(comp, setname.c_str());

				setname = "world_rot." + std::to_string(j + i * 4);
				world_rot.v[i][j] = json_object_dotget_number(comp, setname.c_str());

			}
		}
	}
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

	json_object_dotset_boolean(curr, "properties.fromAINode", false);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			std::string setname = "properties.world_pos." + std::to_string(j + i*4);
			json_object_dotset_number(curr, setname.c_str(), world_pos.v[i][j]);

			setname = "properties.world_rot." + std::to_string(j + i*4);
			json_object_dotset_number(curr, setname.c_str(), world_rot.v[i][j]);

		}
	}

	json_array_append_value(comps, append);
}