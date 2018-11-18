#ifndef _SceneViewer_WINDOW_
#define _SceneViewer_WINDOW_

#include "Window.h"
#include "ModuleScene.h"

class GameObject;

class SceneViewer: public Window
{
public:
	SceneViewer();
	virtual ~SceneViewer();

	void Draw() override;

	void CreateObjLeaf(GameObject* obj, int st);

public:

	bool check_info;
	bool selected_mask;

	int selection_mask;
	int node_clicked;

	int selection_mask_checker;
	
	GameObject* selected_object = nullptr;
	std::vector<int> selection_mask_vec;

	ImVec2 show_size = {128,128};
	
};

#endif

