#ifndef _OCTREEWINDOW_H_
#define _OCTREEWINDOW_H_

#include "Window.h"

class OctreeWindow : public Window
{
public:
	OctreeWindow();
	virtual ~OctreeWindow();

	void Draw() override;

public:
	int elements_per_node, max_subdivisions;
};

#endif
