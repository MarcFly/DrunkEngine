#include "OpenGLOptions.h"
#include "GLEW/include/GL/glew.h"
#include <gl/GL.h>

OpenGLOptions::OpenGLOptions(Application* app) : Window("OpenGL Settings", SDL_SCANCODE_O)
{
	App = app;
	depth_test = cull_face = lighting = color_material = texture_2d = true;
}

void OpenGLOptions::Draw()
{

	ImGui::Begin(GetName().c_str(), &active);
	{

		ImGui::Text("Tinker with the settings we are currently using...");
		ImGui::Separator();

		if (ImGui::Checkbox("Depth Testing", &depth_test))
			Check_Enable_Disable();
		ImGui::SameLine();

		if (ImGui::Checkbox("Cull Faces", &cull_face))
			Check_Enable_Disable();
		ImGui::SameLine();

		if (ImGui::Checkbox("Lighting", &lighting))
			Check_Enable_Disable();
		ImGui::SameLine();

		if (ImGui::Checkbox("Color Materials", &color_material))
			Check_Enable_Disable();

		if (ImGui::Checkbox("Textures 2D", &texture_2d))
			Check_Enable_Disable();
		ImGui::SameLine();

		if (ImGui::Checkbox("Wireframe", &App->renderer3D->wireframe))
			Check_Enable_Disable();
		ImGui::SameLine();
		
	}
	ImGui::End();
}

void OpenGLOptions::Check_Enable_Disable()
{

		if (glIsEnabled(GL_DEPTH_TEST) && depth_test == false)
			glDisable(GL_DEPTH_TEST);
		else if (!glIsEnabled(GL_DEPTH_TEST) && depth_test == true)
			glEnable(GL_DEPTH_TEST);

		if (glIsEnabled(GL_CULL_FACE) && cull_face == false)
			glDisable(GL_CULL_FACE);
		else if (!glIsEnabled(GL_CULL_FACE) && cull_face == true)
			glEnable(GL_CULL_FACE);

		if (glIsEnabled(GL_LIGHTING) && lighting == false)
			glDisable(GL_LIGHTING);
		else if (!glIsEnabled(GL_LIGHTING) && lighting == true)
			glEnable(GL_LIGHTING);

		if (glIsEnabled(GL_COLOR_MATERIAL) && color_material == false)
			glDisable(GL_COLOR_MATERIAL);
		else if (!glIsEnabled(GL_COLOR_MATERIAL) && color_material == true)
			glEnable(GL_COLOR_MATERIAL);

		if (glIsEnabled(GL_TEXTURE_2D) && texture_2d == false)
			glDisable(GL_TEXTURE_2D);
		else if (!glIsEnabled(GL_TEXTURE_2D) && texture_2d == true)
			glEnable(GL_TEXTURE_2D);

}
