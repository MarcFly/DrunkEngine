#ifndef _GEOMETRYCREATIONWINDOW_H_
#define _GEOMETRYCREATIONWINDOW_H_

#include "Window.h"

class GEOMWindow : public Window
{
public:
	GEOMWindow();
	virtual ~GEOMWindow();

	void Draw() override;

public:
	// Used for creation of objects
	// Sphere
	vec sphere_center = { 0,0,0 };
	float sphere_radius = 2;
	bool sphere_phys = false;

	// Cube (AABB)
	vec box_center = { 0,0,0 };
	float box_size = 1;
	bool box_phys = false;
};

#endif