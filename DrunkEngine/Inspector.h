#ifndef _INSPECTOR_WINDOW_
#define _INSPECTOR_WINDOW_

#include "Window.h"
#include "ModuleScene.h"

class GameObject;

class Inspector: public Window
{
public:
	Inspector();
	virtual ~Inspector();

	void Draw() override;

	void CheckMeshInfo();

	void CreateObjLeaf(GameObject* obj, int st);

	void ComponentInspector(Component* component);
	void MeshInspector(ComponentMesh* mesh);
	void MatInspector(ComponentMaterial* mat);
	void CamInspector(ComponentCamera* cam);
	void TransformInspector(ComponentTransform* transform);

	//void GetTotalProperties(GameObject* obj, int &vertex, int &faces);

public:
	int total_num_vertex;
	int total_num_faces;

	std::string tex_name;

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

