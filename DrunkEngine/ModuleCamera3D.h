#ifndef _MODULE_CAMERA_3D_H_
#define _MODULE_CAMERA_3D_H_

#include "Module.h"
#include "Globals.h"
#include "ModuleScene.h"

#include "glmath/glmath.h"

class ComponentCamera;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	bool Load(JSON_Value* root_value);
	bool Save(JSON_Value* root_value);

public:
	
	Color background;
	ComponentCamera * main_camera;

};

#endif