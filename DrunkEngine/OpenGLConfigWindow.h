#ifndef _OPENGLCONFIGWINDOW_H_
#define _OPENGLCONFIGWINDOW_H_

#include "Window.h"
#include "Application.h"

class OpenGLConfigWindow : public Window
{
public:
	OpenGLConfigWindow(Application* app);
	virtual ~OpenGLConfigWindow();

	void Draw() override;

public:
	bool gl_depth_test;
	bool gl_cull_face;
	bool gl_lighting;
	bool gl_color_material;
	bool gl_texture_2d;
	bool gl_extra1;
	bool gl_extra2;
};

#endif
