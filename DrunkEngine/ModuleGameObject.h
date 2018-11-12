#ifndef _MODULE_GAMEOBJECT_H_
#define _MODULE_GAMEOBJECT_H_

#include "Module.h"
#include "ImGuizmo/ImGuizmo.h"

class KDTree;

class ModuleGameObject : public Module
{
public:
	ModuleGameObject(bool start_enabled = true);
	~ModuleGameObject();

	bool Init();
	update_status PreUpdate(float dt);
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
	void AdjustStaticList();

	void SetNonStaticList();

	//KD Tree
	void SetSceneKDTree(const int elements_per_node, const int max_subdivisions);
	KDTree * GetSceneKDTree() const;
	void DeleteSceneKDTree();

	//Root Obj
	void SetRootObject(GameObject * root);
	GameObject * GetRootObject() const;

	void ManageGuizmo();

public:

	std::string scene_folder;

	std::vector<GameObject*> active_objects;
	std::vector<ComponentCamera*> active_cameras;

	std::vector<GameObject*> objects_in_scene;
	std::vector<GameObject*> static_objects_in_scene;
	std::vector<GameObject*> non_static_objects_in_scene;

private:
	GameObject * Root_Object = nullptr;
	KDTree* Scene_KdTree = nullptr;

public:
	GameObject * getRootObj() const { return Root_Object; };
	ComponentCamera * Main_Cam = nullptr;

public:
	//ImGuizmo
	ImGuizmo::OPERATION mCurrentGizmoOperation;
	ImGuizmo::MODE mCurrentGizmoMode;
	
};

#endif // !_MODULE_EVENTSYSTEM_H_