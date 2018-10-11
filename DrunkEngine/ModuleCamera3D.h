#ifndef _MODULE_CAMERA_3D_H_
#define _MODULE_CAMERA_3D_H_

#include "Module.h"
#include "Globals.h"
#include "ModuleManageMesh.h"

#include "glmath/glmath.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	void Transport(const vec3 &Movement);

	void SetToObj(obj_data* obj);

	float* GetViewMatrix();

	bool Load(JSON_Value* root_value);
	bool Save(JSON_Value* root_value);

private:

	void CalculateViewMatrix();

public:
	
	vec3 X, Y, Z, Position, Reference;
	Color background;

	float mesh_multiplier;

private:

	float4x4 ViewMatrix, ViewMatrixInverse;
};

#endif