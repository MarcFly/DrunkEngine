#include "AboutWindow.h"

AboutWindow::AboutWindow() : Window("About...", SDL_SCANCODE_I)
{
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
		ImGui::Text("\nLibraries used: OpenGL (V 2.1)\n				SDL    (V 2.0.8)\n				ImGui  (V 1.66)\n				Bullet (V 2.84)\n\n");
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
