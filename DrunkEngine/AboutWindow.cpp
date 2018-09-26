#include "AboutWindow.h"
#include "GLEW/include/GL/glew.h"
#include <gl/GL.h>

AboutWindow::AboutWindow(Application* app) : Window("About...", SDL_SCANCODE_I)
{
	App = app;
}

AboutWindow::~AboutWindow()
{
}

void AboutWindow::Draw()
{
	ImGui::Begin(GetName().c_str(), &active);
	{
		ImGui::Text("DrunkEngine (2018).");
		ImGui::Separator();
		ImGui::TextWrapped("\nThis engine has been made as a project for a 3rd course subject on videogame development.\n\n");
		ImGui::Text("Authors: Andreu Sacasas\n         Marc Torres\n\n");
		ImGui::Separator();

		// Get Library Versions
		SDL_version sdl_ver;
		SDL_GetVersion(&sdl_ver);
		int major, minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);
		ImGui::Text("\nLibraries used: OpenGL (%d.%d)\n				SDL    (%d.%d.%d)\n				ImGui  (%.2f)\n				Bullet (%.2f)\n				GLEW (%d.%d.%d)\n\n", major, minor, sdl_ver.major, sdl_ver.minor, sdl_ver.patch, IMGUI_VERSION_NUM / 10000.0f, btGetVersion() / 100.0f, GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);
		
		ImGui::Separator();

		ImGui::TextWrapped("\nMIT License\n\nCopyright(c) 2018 Marc Torres Jimenez & Andreu Sacasas Velazquez\n\n"
			"Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the 'Software'), to deal "
			"in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell "
			"copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :\n\n"
			"The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. "
			"THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
			"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
			"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.'");
	}
	ImGui::End();
}
