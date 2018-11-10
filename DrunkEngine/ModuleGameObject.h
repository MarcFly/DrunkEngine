#ifndef _MODULE_GAMEOBJECT_H_
#define _MODULE_GAMEOBJECT_H_

#include "Module.h"

class KDTree;

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

	void SetSceneObjects(GameObject * obj);

	void SetToStaticObjects(GameObject * obj);
	void DeleteFromStaticObjects(GameObject * obj);
	void AdjustVectorList();

	//KD Tree
	void SetSceneKDTree(const int elements_per_node, const int max_subdivisions);
	KDTree * GetSceneKDTree() const;

	//Root Obj
	void SetRootObject(GameObject * root);
	GameObject * GetRootObject() const;

public:

	std::string scene_folder;

	std::vector<GameObject*> active_objects;
	std::vector<ComponentCamera*> active_cameras;

	std::vector<GameObject*> objects_in_scene;
	std::vector<GameObject*> static_objects_in_scene;

private:
	GameObject * Root_Object = nullptr;
	KDTree* Scene_KdTree = nullptr;

public:
	GameObject * getRootObj() const { return Root_Object; };
	ComponentCamera * Main_Cam = nullptr;

	
};

#endif // !_MODULE_EVENTSYSTEM_H_