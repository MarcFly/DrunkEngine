#ifndef _COMPONENT_TRANSFORM_
#define _COMPONENT_TRANSFORM_

#include "GameObject.h"

class ComponentTransform
{
public:
	ComponentTransform() {};
	ComponentTransform(aiMatrix4x4* t, GameObject* par);

	~ComponentTransform() {};
public:

	aiVector3D transform_position;
	aiVector3D transform_scale;
	aiQuaternion transform_rotate;

	GameObject* parent = nullptr;

};

#endif