#ifndef _GEOPROPERTIES_WINDOW_
#define _GEOPROPERTIES_WINDOW_

#include "Window.h"
#include "ModuleManageMesh.h"

class GeoPropertiesWindow: public Window
{
public:
	GeoPropertiesWindow();
	virtual ~GeoPropertiesWindow();

	void Draw() override;

	void CheckMeshInfo();

public:
	int total_num_vertex;
	int total_num_faces;

	std::string tex_name;

	bool check_info;

	ImVec2 show_size = {128,128};
	
};

#endif

