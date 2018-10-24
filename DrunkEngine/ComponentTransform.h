#ifndef _COMPONENT_TRANSFORM_
#define _COMPONENT_TRANSFORM_

#include "MGL/MathGeoLib.h"
#include "Assimp/include/scene.h"
#include "Component.h"

class GameObject;
class ComponentMesh;

class ComponentTransform : Component
{
public:
	ComponentTransform() {};
	ComponentTransform(const aiMatrix4x4* t, GameObject* par);
	ComponentTransform(const aiMatrix4x4* t, ComponentMesh* par);

	~ComponentTransform() {};

	void SetTransformPosition(const int pos_x, const int pos_y, const int pos_z);
	void SetTransformRotation(const Quat rot_quat);
	void SetTransformRotation(const float3 rot_vec);
	void SetTransformScale(const int scale_x, const int scale_y, const int scale_z);

	void CleanUp();

public:

	float3 transform_position;
	float3 transform_scale;
	Quat transform_rotate_quat;
	float3 transform_rotate_euler;

	GameObject* parent = nullptr;
	ComponentMesh* mparent = nullptr;

};

#endif