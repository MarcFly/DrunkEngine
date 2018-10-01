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
	vec sphere_center;
	float sphere_radius;
	bool sphere_phys = false;

	// Cube (AABB)
	vec box_center;
	float box_size;
	bool box_phys = false;
};

#endif