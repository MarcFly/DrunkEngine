#ifndef _COMPONENT_CAMERA_H_
#define _COMPONENT_CAMERA_H_

#include "Module.h"
#include "MGL/MathGeoLib.h"

class GameObject;

class ComponentCamera
{
public:
	ComponentCamera(GameObject * gameobject);
	virtual ~ComponentCamera();

	void Start();
	bool Update(float dt);
	void Draw();
	void CleanUp();;

	void SetAspectRatio();

	float* GetViewMatrix();

	void MoveTest(float speed);

	bool Load(JSON_Value* root_value);
	bool Save(JSON_Value* root_value);

private:
	void CalculateViewMatrix();

private:
	GameObject * parent = nullptr;
	
	float3 bb_frustum[8];

	float4x4 ViewMatrix, ViewMatrixInverse;

	vec X, Y, Z;
	vec Position, Reference;

public:
	bool projection_update;
	Frustum frustum;
};


#endif // !_COMPONENT_CAMERA_H_

