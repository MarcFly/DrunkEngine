#ifndef _MODULE_GAMEOBJECT_H_
#define _MODULE_GAMEOBJECT_H_

#include "Module.h"

class ModuleGameObject : public Module
{
public:
	ModuleGameObject(bool start_enabled = true);
	~ModuleGameObject();

	update_status Update(float dt);
	bool CleanUp();

	void Draw();

	void SetActiveFalse();
	void SetmainCam(ComponentCamera * cam);

	void NewScene();
	void CreateMainCam();
	void DeleteScene();

public:
	GameObject * Root_Object = nullptr;

	std::string scene_folder;

	std::vector<GameObject*> active_objects;
	std::vector<ComponentCamera*> active_cameras;

public:
	GameObject * getRootObj() const { return Root_Object; };
	ComponentCamera * Main_Cam = nullptr;

	
};

#endif // !_MODULE_EVENTSYSTEM_H_