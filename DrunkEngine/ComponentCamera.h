#ifndef _COMPONENT_CAMERA_H_
#define _COMPONENT_CAMERA_H_

#include "Module.h"
#include "MGL/MathGeoLib.h"
#include "Component.h"

class GameObject;

class ComponentCamera : public Component
{
public:
	ComponentCamera(GameObject * gameobject);
	virtual ~ComponentCamera();

	void Start();
	void Update(const float dt);
	void Draw();
	void CleanUp();;

	void LookAt(const vec &Spot);
	void Move(const vec &Movement);
	void MoveZ(const float mov);
	void MoveX(const float mov);
	void Transport(const vec &Movement);
	void Rotate();
	void RotateAround(const float3 aux);
	float3 RotateAngle(const float3 &u, float angle, const float3 &v);

	void SetAspectRatio();

	void CalculateViewMatrix();
	float* GetViewMatrix();

	void LookToObj(GameObject* obj, const float vertex_aux);
	void ComponentCamera::LookToActiveObjs(const vec look_to);

	void TransformPos(float3 pos);
	void TransformRot(Quat rot);
	void TransformScale(float3 scale);

	void SetbbFrustum();

	void Load(const JSON_Object* comp);
	void Save(JSON_Array* comps);

private:
	float3 bb_frustum[8];

public:
	int id;
	float mesh_multiplier;

	float4x4 ViewMatrix, ViewMatrixInverse;

	vec X, Y, Z;
	vec Reference;

	Frustum frustum;

	//float original_v_fov;		If scale is compatible whith camera
	//float original_h_fov;

public:
	void SetBaseVals()
	{
		type = CT_Camera;
		multiple = false;
		parent = nullptr;
	}
};


#endif // !_COMPONENT_CAMERA_H_

