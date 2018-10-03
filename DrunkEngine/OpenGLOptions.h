#ifndef _OpenGLOptions_H_
#define _OpenGLOptions_H_

#include "Window.h"

class OpenGLOptions : public Window
{
public:
	OpenGLOptions(Application* app);
	~OpenGLOptions() {};

	void Draw();

	void Check_Enable_Disable();

	bool depth_test, cull_face, lighting, color_material, texture_2d, wireframe;

	bool GetWireframe() { return wireframe; };
};

#endif