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
	int num_vertex;
	int num_faces;

	bool check_info;

	ImVec2 show_size = {128,128};
	
};

#endif

