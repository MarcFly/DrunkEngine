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

	void MousePicking();
	void TestIntersect(GameObject* obj, LineSegment& ray, std::vector<GameObject*>& intersected);
	float TestTris(LineSegment local, ComponentMesh* mesh);

	void DrawRay(vec a, vec b);

public:	
	Color background;
	ComponentCamera * main_camera;

private:
	int win_w;
	int win_h;

	LineSegment picking = LineSegment(vec::zero, vec::zero);
};

#endif