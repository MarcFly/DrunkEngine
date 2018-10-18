#ifndef _COMPONENT_TRANSFORM_
#define _COMPONENT_TRANSFORM_

#include "MGL/MathGeoLib.h"
#include "Assimp/include/scene.h"

class GameObject;
class ComponentMesh;

class ComponentTransform
{
public:
	ComponentTransform() {};
	ComponentTransform(const aiMatrix4x4* t, GameObject* par);
	ComponentTransform(const aiMatrix4x4* t, ComponentMesh* par);

	~ComponentTransform() {};

	void TransformPosition(const int pos_x, const int pos_y, const int pos_z);
	void TransformRotation(const int rot_x, const int rot_y, const int rot_z);
	void TransformScale(const int scale_x, const int scale_y, const int scale_z);

	void CleanUp();

public:

	aiVector3D transform_position;
	aiVector3D transform_scale;
	aiQuaternion transform_rotate;

	GameObject* parent = nullptr;
	ComponentMesh* mparent = nullptr;

};

#endif