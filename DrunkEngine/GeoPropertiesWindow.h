#ifndef _GEOPROPERTIES_WINDOW_
#define _GEOPROPERTIES_WINDOW_

#include "Window.h"
#include "ModuleScene.h"

class GameObject;

class GeoPropertiesWindow: public Window
{
public:
	GeoPropertiesWindow();
	virtual ~GeoPropertiesWindow();

	void Draw() override;

	void CheckMeshInfo();

	void CreateObjLeaf(GameObject* obj, int st);

public:
	int total_num_vertex;
	int total_num_faces;

	std::string tex_name;

	bool check_info;

	int selection_mask;
	int node_clicked;

	int selection_mask_checker;
	
	GameObject* selected_object;

	ImVec2 show_size = {128,128};
	
};

#endif

