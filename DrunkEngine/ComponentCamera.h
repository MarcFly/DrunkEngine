#ifndef _COMPONENT_CAMERA_H_
#define _COMPONENT_CAMERA_H_

#include "Module.h"

class GameObject;
class ModuleCamera3D;

class ComponentCamera
{
public:
	ComponentCamera();
	virtual ~ComponentCamera();

	void Start();
	update_status Update();
	void CleanUp();;

	bool Load(JSON_Value* root_value);
	bool Save(JSON_Value* root_value);

private:
	ModuleCamera3D * camera = nullptr;
};


#endif // !_COMPONENT_CAMERA_H_

