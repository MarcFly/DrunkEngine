#ifndef _COMPONENT_TRANSFORM_
#define _COMPONENT_TRANSFORM_

#include "MGL/MathGeoLib.h"
#include "Assimp/include/scene.h"
#include "Component.h"

class GameObject;
class ComponentMesh;

class ComponentTransform : public Component
{
public:
	ComponentTransform(GameObject* par) { parent = par; SetBaseVals();  }
	ComponentTransform(const aiMatrix4x4* t, GameObject* par);

	~ComponentTransform() {};

	void SetTransformPosition(const float pos_x, const float pos_y, const float pos_z);
	void SetTransformRotation(const Quat rot_quat);
	void SetTransformRotation(const float3 rot_vec);
	void SetTransformScale(const float scale_x, const float scale_y, const float scale_z);

	void SetLocalTransform();

	void RecursiveSetChildrenToUpdate(ComponentTransform* t = nullptr);
	void RecursiveSetParentToUpdate(ComponentTransform* t = nullptr);

	void SetFromMatrix(const aiMatrix4x4* t);

	void SetWorldPos(const float4x4 new_transform);
	void SetWorldRot(const Quat new_rot);

	void CalculateGlobalTransforms();

	void SetAuxWorldPos();

	void CleanUp();

	void Load(const JSON_Object* comp);
	void Save(JSON_Array* comps);

public:

	float3 position;
	float3 scale;
	Quat rotate_quat;
	float3 rotate_euler;

	float4x4 local_transform;
	float4x4 global_transform;

	bool update_bounding_box;

	float4x4 world_pos;		//Initialized as (0,0,0), this one is used to calculate the real position
	float4x4 world_rot;

	float4x4 aux_world_pos;		//The user sees this one

	float3 global_pos;
	Quat global_rot;
	float3 global_scale;

public:
	void SetBaseVals()
	{
		global_scale, global_pos, position = { 0,0,0 };
		scale = { 1,1,1 }; 
		rotate_euler = { 0,0,0 };
		aux_world_pos = world_rot = world_pos = float4x4::identity;
		SetTransformRotation(rotate_euler);
		type = CT_Transform;
		multiple = false;
    
		update_bounding_box = true;
	}

};

#endif