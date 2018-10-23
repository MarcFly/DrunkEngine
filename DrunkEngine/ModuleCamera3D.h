#ifndef _MODULE_CAMERA_3D_H_
#define _MODULE_CAMERA_3D_H_

#include "Module.h"
#include "Globals.h"
#include "ModuleScene.h"

#include "glmath/glmath.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec &Position, const vec &Reference, bool RotateAroundReference = false);
	void LookAt(const vec &Spot);
	void Move(const vec &Movement);
	void Transport(const vec &Movement);
	void Rotate();
	float3 RotateAngle(const float3 &u, float angle, const float3 &v);

	void SetToObj(GameObject* obj, float vertex_aux);

	float* GetViewMatrix();

	bool Load(JSON_Value* root_value);
	bool Save(JSON_Value* root_value);

private:

	void CalculateViewMatrix();

public:
	
	vec X, Y, Z, Position, Reference;
	Color background;

	float mesh_multiplier;

private:

	float4x4 ViewMatrix, ViewMatrixInverse;
};

#endif