#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "Globals.h"
#include "Application.h"
#include "SDL\include\SDL_scancode.h"
#include <string>

class Window
{
public:
	Window(const char* name, SDL_Scancode shortcut);
	virtual ~Window();

	void SwitchActive();
	bool IsActive() const;
	std::string GetName() const;

	SDL_Scancode GetShortCut() const;
	virtual void Draw() = 0;

public:
	bool active = false;
	Application* App;

protected:
	std::string name;
	SDL_Scancode shortcut = SDL_SCANCODE_UNKNOWN;
};

#endif