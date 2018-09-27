#include "OpenGLConfigWindow.h"
#include "SDL\include\SDL_opengl.h"

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
			if (gl_depth_test)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}
		ImGui::SameLine();

		if (ImGui::Checkbox("GL_Cull_Face", &gl_cull_face))
		{
			if (gl_cull_face)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}
		ImGui::SameLine();

		if (ImGui::Checkbox("GL_Lighting", &gl_lighting))
		{
			if (gl_lighting)
				glEnable(GL_LIGHTING);
			else
				glDisable(GL_LIGHTING);
		}

		ImGui::Separator();

		if (ImGui::Checkbox("GL_Color_Material", &gl_color_material))
		{
			if (gl_color_material)
				glEnable(GL_COLOR_MATERIAL);
			else
				glDisable(GL_COLOR_MATERIAL);
		}
		ImGui::SameLine();

		if (ImGui::Checkbox("GL_Texture_2D", &gl_texture_2d))
		{
			if (gl_texture_2d)
				glEnable(GL_TEXTURE_2D);
			else
				glDisable(GL_TEXTURE_2D);
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
