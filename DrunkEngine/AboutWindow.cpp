#include "AboutWindow.h"
#include "GLEW/include/GL/glew.h"
#include <gl/GL.h>
#include "ModuleManageMesh.h"

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

		// Get Library Versions
		SDL_version sdl_ver;
		SDL_GetVersion(&sdl_ver);
		int major, minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);
	
		ImGui::Text("LIBRARIES USED:");
		ImGui::TextColored(HyperlinkColor,"OpenGL (%d.%d) ", major, minor);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Go to webpage");
		if (ImGui::IsItemClicked())
			ShellExecute(NULL, "open", "https://www.opengl.org/", NULL, NULL, SW_SHOWNORMAL);
		
		ImGui::SameLine();
		ImGui::TextColored(HyperlinkColor, "SDL (%d.%d.%d) ", sdl_ver.major, sdl_ver.minor, sdl_ver.patch);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Go to webpage");
		if (ImGui::IsItemClicked())
			ShellExecute(NULL, "open", "https://www.libsdl.org/", NULL, NULL, SW_SHOWNORMAL);
		
		ImGui::SameLine();
		ImGui::TextColored(HyperlinkColor, "ImGui  (%.2f) ", IMGUI_VERSION_NUM / 10000.0f);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Go to webpage");
		if (ImGui::IsItemClicked())
			ShellExecute(NULL, "open", "https://github.com/ocornut/imgui", NULL, NULL, SW_SHOWNORMAL);
		
		/*ImGui::SameLine();
		ImGui::TextColored(HyperlinkColor, "Bullet (%.2f) ", btGetVersion() / 100.0f);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Go to webpage");
		if (ImGui::IsItemClicked())
			ShellExecute(NULL, "open", "https://github.com/bulletphysics", NULL, NULL, SW_SHOWNORMAL);
*/
		ImGui::TextColored(HyperlinkColor, "GLEW(%d.%d.%d ", GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Go to webpage");
		if (ImGui::IsItemClicked())
			ShellExecute(NULL, "open", "http://glew.sourceforge.net/", NULL, NULL, SW_SHOWNORMAL);
		
		ImGui::SameLine();
		ImGui::TextColored(HyperlinkColor, "DevIL (%d.%d.%D) ", App->mesh_loader->GetDevILVer()/100, App->mesh_loader->GetDevILVer() / 10 - 10*(App->mesh_loader->GetDevILVer() / 100), App->mesh_loader->GetDevILVer() - 10 * (App->mesh_loader->GetDevILVer() /10));
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Go to webpage");
		if (ImGui::IsItemClicked())
			ShellExecute(NULL, "open", "http://openil.sourceforge.net/", NULL, NULL, SW_SHOWNORMAL);
		
		ImGui::SameLine();
		ImGui::TextColored(HyperlinkColor, "Assimp (%d.%d.%d) ", App->mesh_loader->GetAssimpMajorVer(), App->mesh_loader->GetAssimpMinorVer(), App->mesh_loader->GetAssimpVersionRevision());
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Go to webpage");
		if (ImGui::IsItemClicked())
			ShellExecute(NULL, "open", "http://www.assimp.org/", NULL, NULL, SW_SHOWNORMAL);
		
		ImGui::SameLine();
		ImGui::TextColored(HyperlinkColor, "MathGeoLib ");
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Go to webpage");
		if (ImGui::IsItemClicked())
			ShellExecute(NULL, "open", "https://github.com/juj/MathGeoLib", NULL, NULL, SW_SHOWNORMAL);

		ImGui::TextColored(HyperlinkColor, "GLMATH ");
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Go to webpage");
		if (ImGui::IsItemClicked())
			ShellExecute(NULL, "open", "http://glmath.sourceforge.net/", NULL, NULL, SW_SHOWNORMAL);
		
		ImGui::SameLine();
		ImGui::TextColored(HyperlinkColor, "PCG_RNG ");
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Go to webpage");
		if (ImGui::IsItemClicked())
			ShellExecute(NULL, "open", "http://www.pcg-random.org/", NULL, NULL, SW_SHOWNORMAL);
		
		ImGui::SameLine();
		ImGui::TextColored(HyperlinkColor, "Parson" );
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Go to webpage");
		if (ImGui::IsItemClicked())
			ShellExecute(NULL, "open", "https://github.com/kgabis/parson", NULL, NULL, SW_SHOWNORMAL);

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
