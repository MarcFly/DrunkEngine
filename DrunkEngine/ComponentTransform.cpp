#include "ComponentTransform.h"
#include "Assimp/include/scene.h"

ComponentTransform::ComponentTransform(const aiMatrix4x4 * t, GameObject* par)
{
	//aiQuaternion rotation_quat; = rotation_quat.GetEuler();
	t->Decompose(this->transform_scale, this->transform_rotate, this->transform_position);

	this->parent = par;
}

ComponentTransform::ComponentTransform(const aiMatrix4x4 * t, ComponentMesh* par)
{
	//aiQuaternion rotation_quat; = rotation_quat.GetEuler();
	t->Decompose(this->transform_scale, this->transform_rotate, this->transform_position);
	this->mparent = par;
}

void ComponentTransform::SetTransformPosition(const int pos_x, const int pos_y, const int pos_z)
{
	transform_position.x = pos_x;
	transform_position.y = pos_y;
	transform_position.z = pos_z;

}

void ComponentTransform::SetTransformRotation(const int rot_x, const int rot_y, const int rot_z)
{
	vec rot_mat(DegToRad(rot_x), DegToRad(rot_y), DegToRad(rot_z));

	transform_rotate = toQuaternion(rot_mat);
}

void ComponentTransform::SetTransformScale(const int scale_x, const int scale_y, const int scale_z)
{	
	transform_scale.x = scale_x;
	transform_scale.y = scale_y;
	transform_scale.z = scale_z;
}

void ComponentTransform::CleanUp()
{
	this->parent = nullptr;
}

aiQuaternion ComponentTransform::toQuaternion(vec transform)
{
	aiQuaternion q;
	// Abbreviations for the various angular functions
	float cy = cos(transform.z * 0.5);
	float sy = sin(transform.z * 0.5);
	float cr = cos(transform.y * 0.5);
	float sr = sin(transform.y * 0.5);
	float cp = cos(transform.x * 0.5);
	float sp = sin(transform.x * 0.5);

	q.w = cy * cr * cp + sy * sr * sp;
	q.x = cy * sr * cp - sy * cr * sp;
	q.y = cy * cr * sp + sy * sr * cp;
	q.z = sy * cr * cp - cy * sr * sp;
	return q;
}