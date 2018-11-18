#ifndef _MODULE_CAMERA_3D_H_
#define _MODULE_CAMERA_3D_H_

#include "Module.h"
#include "Globals.h"
#include "ModuleScene.h"
#include "KdTree.h"

#include "glmath/glmath.h"

class ComponentCamera;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

	bool Load(const JSON_Value* root_value);
	bool Save(JSON_Value* root_value);
	
	void MousePicking();
	void TestIntersect(const std::vector<GameObject*>& objs, const LineSegment & ray, std::vector<GameObject*>& intersected);
	void TreeTestIntersect(const KDTree::Node* node, const LineSegment& ray, std::vector<GameObject*>& objects_to_check);
	float TestTris(LineSegment local, const ComponentMesh* mesh);

	void DrawRay(vec a, vec b) const;

	void RecieveEvent(const Event & event);

	void SetMainCamAspectRatio();

public:	
	Color background;
	ComponentCamera * main_camera;

private:
	LineSegment picking = LineSegment(vec::zero, vec::zero);
};

#endif