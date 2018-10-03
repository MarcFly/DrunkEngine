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

		ImGui::Checkbox("Depth Testing", &depth_test);
		ImGui::SameLine();

		ImGui::Checkbox("Cull Faces", &cull_face);
		ImGui::SameLine();

		ImGui::Checkbox("Lighting", &lighting);
		ImGui::SameLine();

		ImGui::Checkbox("Color Materials", &color_material);
		ImGui::SameLine();

		ImGui::Checkbox("Textures 2D", &texture_2d);
		ImGui::SameLine();
		
	}
	ImGui::End();

	Check_Enable_Disable();
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
