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

	void Look(const vec &Position, const vec &Reference, bool RotateAroundReference);
	void LookAt(const vec &Spot);
	void Move(const vec &Movement);
	void Transport(const vec &Movement);
	void Rotate();
	float3 RotateAngle(const float3 &u, float angle, const float3 &v);

	void SetAspectRatio();

	void CalculateViewMatrix();
	float* GetViewMatrix();

	void MoveTest(float speed);

	void LookToObj(GameObject* obj, float vertex_aux);

	void Load(JSON_Object* comp);
	void Save(JSON_Array* comps);

private:
	float3 bb_frustum[8];

public:
	int id;
	float mesh_multiplier;

	float4x4 ViewMatrix, ViewMatrixInverse;

	vec X, Y, Z;
	vec Position, Reference;

	Frustum frustum;

public:
	void SetBaseVals()
	{
		type = CT_Camera;
		multiple = true;
		parent = nullptr;
	}
};


#endif // !_COMPONENT_CAMERA_H_

