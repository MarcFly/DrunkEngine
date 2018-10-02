#include "ConsoleWindow.h"

ConsoleWindow::ConsoleWindow(Application * app) : Window("Console", SDL_SCANCODE_UNKNOWN)
{
	App = app;
}

ConsoleWindow::~ConsoleWindow()
{
}

void ConsoleWindow::Draw()
{

}
