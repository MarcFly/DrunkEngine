#ifndef _GEOMETRYCREATIONWINDOW_H_
#define _GEOMETRYCREATIONWINDOW_H_

#include "Window.h"

class GEOMWindow : public Window
{
public:
	GEOMWindow(Application* app);
	virtual ~GEOMWindow();

	void Draw() override;

public:
	// Used for creation of objects
	// Sphere
	vec sphere_center = { 0,0,0 };
	float sphere_radius = 0;
	bool sphere_phys = false;

	// Cube (AABB)
	vec box_center = { 0,0,0 };
	float box_size = 0;
	bool box_phys = false;
};

#endif