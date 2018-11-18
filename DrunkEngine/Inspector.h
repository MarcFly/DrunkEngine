#ifndef _INSPECTOR_WINDOW_
#define _INSPECTOR_WINDOW_

#include "Window.h"
#include "ModuleScene.h"

class GameObject;

class Inspector : public Window
{
public:
	Inspector();
	virtual ~Inspector();

	void Draw() override;

	void CheckMeshInfo();

	void ComponentInspector(Component* component);
	void MeshInspector(ComponentMesh* mesh);
	void MatInspector(ComponentMaterial* mat);
	void CamInspector(ComponentCamera* cam);
	void TransformInspector(ComponentTransform* transform);

public:
	int total_num_vertex;
	int total_num_faces;

	std::string tex_name;

	bool check_info;

	bool radio_world = false;
	bool radio_local = true;

	ImVec2 show_size = { 128,128 };

};

#endif
