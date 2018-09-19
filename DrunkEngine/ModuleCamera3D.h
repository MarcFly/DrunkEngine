#ifndef _MODULE_CAMERA_3D_H_
#define _MODULE_CAMERA_3D_H_

#include "Module.h"
#include "Globals.h"
#include "MGL\MathGeoLib.h"
#include "MGL\MathGeoLibFwd.h"

#include "Primitive.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const btVector3 &Position, const btVector3 &Reference, bool RotateAroundReference = false);
	void LookAt(const btVector3 &Spot);
	void Move(const btVector3 &Movement);
	float* GetViewMatrix();

private:

	void CalculateViewMatrix();

public:
	
	btVector3 X, Y, Z, Position, Reference;

private:

	btMatrixX<float> ViewMatrix = btMatrixX<float>(4, 4), ViewMatrixInverse = btMatrixX<float>(4, 4);
};

#endif