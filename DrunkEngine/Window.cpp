#include "Window.h"

Window::Window(const char * name)
{
	this->name = name;
}

Window::~Window()
{
}

void Window::SwitchActive()
{
	active = !active;
}

bool Window::IsActive() const
{
	return active;
}

std::string Window::GetName() const
{
	return name;
}

SDL_Scancode Window::GetShortCut() const
{
	return shortcut;
}


