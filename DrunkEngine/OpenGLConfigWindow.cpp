#include "OpenGLConfigWindow.h"

OpenGLConfigWindow::OpenGLConfigWindow(Application * app) : Window("OpenGL Configuration", SDL_SCANCODE_UNKNOWN)
{
	App = app;

	gl_depth_test = true;
	gl_cull_face = true;
	gl_lighting = true;
	gl_color_material = true;
	gl_texture_2d = true;
	gl_extra1 = true;
	gl_extra2 = true;
}

OpenGLConfigWindow::~OpenGLConfigWindow()
{
}

void OpenGLConfigWindow::Draw()
{
	ImGui::Begin(GetName().c_str(), &active);
	{
		if (ImGui::Checkbox("GL_Depth_Test", &gl_depth_test))
		{
			
		}
		ImGui::SameLine();

		if (ImGui::Checkbox("GL_Cull_Face", &gl_cull_face))
		{

		}
		ImGui::SameLine();

		if (ImGui::Checkbox("GL_Lighting", &gl_lighting))
		{

		}

		ImGui::Separator();

		if (ImGui::Checkbox("GL_Color_Material", &gl_color_material))
		{

		}
		ImGui::SameLine();

		if (ImGui::Checkbox("GL_Texture_2D", &gl_texture_2d))
		{

		}
		ImGui::SameLine();

		if (ImGui::Checkbox("GL_Extra1", &gl_extra1))
		{
			
		}
		
		if (ImGui::Checkbox("GL_Extra2", &gl_extra2))
		{

		}
	}
	ImGui::End();
}
