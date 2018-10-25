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
	ComponentTransform() { position = { 0,0,0 }; scale = { 1,1,1 }; rotate_euler = { 0,0,0 }; SetBaseVals(); };
	ComponentTransform(const aiMatrix4x4* t, GameObject* par);
	ComponentTransform(const aiMatrix4x4* t, ComponentMesh* par);

	~ComponentTransform() {};

	void SetTransformPosition(const int pos_x, const int pos_y, const int pos_z);
	void SetTransformRotation(const Quat rot_quat);
	void SetTransformRotation(const float3 rot_vec);
	void SetTransformScale(const int scale_x, const int scale_y, const int scale_z);

	void CleanUp();

public:

	float3 position;
	float3 scale;
	Quat rotate_quat;
	float3 rotate_euler;

	GameObject* parent = nullptr;
	ComponentMesh* mparent = nullptr;


public:
	void SetBaseVals()
	{
		type = CT_Transform;
		multiple = false;
	}
};

#endif