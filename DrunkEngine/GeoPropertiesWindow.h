#ifndef _GEOPROPERTIES_WINDOW_
#define _GEOPROPERTIES_WINDOW_

#include "Window.h"

class GeoPropertiesWindow: public Window
{
public:
	GeoPropertiesWindow(Application* app);
	virtual ~GeoPropertiesWindow();

	void Draw() override;

	//void UpdateMeshInfo();

public:
	int num_vertex;
	int num_faces;
	
};

#endif

