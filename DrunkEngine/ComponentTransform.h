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
	ComponentTransform() { SetBaseVals(); };
	ComponentTransform(GameObject* par) :parent{ par } { SetBaseVals(); }
	ComponentTransform(const aiMatrix4x4* t, GameObject* par);
	ComponentTransform(const aiMatrix4x4* t, ComponentMesh* par);

	~ComponentTransform() {};

	void SetTransformPosition(const int pos_x, const int pos_y, const int pos_z);
	void SetTransformRotation(const Quat rot_quat);
	void SetTransformRotation(const float3 rot_vec);
	void SetTransformScale(const int scale_x, const int scale_y, const int scale_z);

	void SetFromMatrix(const aiMatrix4x4* t);

	void CleanUp();

	void Load(JSON_Object* comp);
	void Save(JSON_Array* comps);

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
		position = { 0,0,0 }; 
		scale = { 1,1,1 }; 
		rotate_euler = { 0,0,0 };
		type = CT_Transform;
		multiple = false;
	}
};

#endif