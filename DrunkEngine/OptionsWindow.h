#ifndef _OPTIONSWINDOW_H_
#define _OPTIONSWINDOW_H_

#include "Window.h"
#include "Application.h"
#include "p2DynArray.h"

class OptionsWindow : public Window
{
public:
	OptionsWindow(Application* app);
	virtual ~OptionsWindow();

	void Draw() override;

public:

	int width;
	int height;

	float brightness;
	bool fullscreen;
	bool resizable;
	bool borderless;
	bool full_desktop;

	float fps;
	float max_fps;
	std::string log_fps;

	Timer frame_read_time;

	p2DynArray<float> fps_history;
};

#endif
