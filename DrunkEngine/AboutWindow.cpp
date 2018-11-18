#include "AboutWindow.h"
#include "GLEW/include/GL/glew.h"
#include <gl/GL.h>
#include "ModuleScene.h"

AboutWindow::AboutWindow() : Window("About...")
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
		ImGui::Text("AUTHORS: ");
		ImGui::SameLine();
		ImGui::TextColored(AndreuColor,"Andreu Sacasas   ");
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Go to Github");
		if (ImGui::IsItemClicked())
			ShellExecute(NULL, "open", "https://github.com/Andreu997", NULL, NULL, SW_SHOWNORMAL);
		
		ImGui::SameLine();
		ImGui::TextColored(MarcColor, "Marc Torres");
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Go to Github");
		if (ImGui::IsItemClicked())
			ShellExecute(NULL, "open", "https://github.com/MarcFly", NULL, NULL, SW_SHOWNORMAL);

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
