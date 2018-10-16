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

	void CleanUp();

public:

	aiVector3D transform_position;
	aiVector3D transform_scale;
	aiQuaternion transform_rotate;

	GameObject* parent = nullptr;
	ComponentMesh* mparent = nullptr;

};

#endif