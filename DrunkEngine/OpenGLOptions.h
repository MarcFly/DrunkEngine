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
};

#endif