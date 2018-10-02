#ifndef _CONSOLEWINDOW_H_
#define _CONSOLEWINDOW_H_

#include "Window.h"
#include "Application.h"
#include <vector>

class ConsoleWindow : public Window
{
public:
	ConsoleWindow(Application* app);
	virtual ~ConsoleWindow();

	void Draw() override;

public:

	std::vector<char> log_history;
};

#endif