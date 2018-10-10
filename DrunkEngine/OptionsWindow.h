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

	void CheckInputChange();

public:

	float fps;
	float max_fps;

	Timer frame_read_time;
	Timer repeated_key_time;

	std::vector<float> fps_history;
	std::vector<float> dt_history;

	int input_change;
	bool key_repeated;
};

#endif
