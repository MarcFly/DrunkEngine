#ifndef _KDTreeWindow_H_
#define _KDTreeWindow_H_

#include "Window.h"

class KDTreeWindow : public Window
{
public:
	KDTreeWindow();
	virtual ~KDTreeWindow();

	void Draw() override;

public:
	int elements_per_node, max_subdivisions;
};

#endif
