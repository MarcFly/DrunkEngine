#include "ComponentTransform.h"

ComponentTransform::ComponentTransform(aiMatrix4x4 * t, GameObject* par)
{
	//aiQuaternion rotation_quat; = rotation_quat.GetEuler();
	t->Decompose(this->transform_scale, this->transform_rotate, this->transform_position);

	this->parent = par;
}
