#ifndef _GEOTRANSFORM_WINDOW_
#define _GEOTRANSFORM_WINDOW_

#include "Window.h"

class GeoTransformWindow : public Window
{
public:
	GeoTransformWindow(Application* app);
	virtual ~GeoTransformWindow();

	void Draw() override;


public:
	
};

#endif // !_GEOTRANSFORM_WINDOW_

