#ifndef _COMPONENT_TRANSFORM_
#define _COMPONENT_TRANSFORM_

#include "MGL/MathGeoLib.h"
#include "Assimp/include/scene.h"

class GameObject;
class ComponentMesh;

class ComponentTransform
{
public:
	ComponentTransform();
	ComponentTransform(const aiMatrix4x4* t, GameObject* par);
	ComponentTransform(const aiMatrix4x4* t, ComponentMesh* par);

	~ComponentTransform() {};

	void SetTransformPosition(const float pos_x, const float pos_y, const float pos_z);
	void SetTransformRotation(const Quat rot_quat);
	void SetTransformRotation(const float3 rot_vec);
	void SetTransformScale(const float scale_x, const float scale_y, const float scale_z);

	void SetLocalTransform();

	void CleanUp();

public:

	float4x4 local_transform;
	float4x4 global_transform;

	float3 transform_position;
	float3 transform_scale;
	Quat transform_rotate_quat;
	float3 transform_rotate_euler;

	GameObject* parent = nullptr;
	ComponentMesh* mparent = nullptr;

};

#endif