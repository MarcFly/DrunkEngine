#ifndef _OPTIONSWINDOW_H_
#define _OPTIONSWINDOW_H_

#include "Window.h"
#include "Application.h"
#include <vector>

class OptionsWindow : public Window
{
public:
	OptionsWindow(Application* app);
	virtual ~OptionsWindow();

	void Draw() override;

public:

	float fps;
	float max_fps;

	Timer frame_read_time;

	std::vector<float> fps_history;
	std::vector<float> dt_history;
};

#endif
