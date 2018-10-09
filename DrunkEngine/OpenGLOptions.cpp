#include "OpenGLOptions.h"
#include "GLEW/include/GL/glew.h"
#include <gl/GL.h>

OpenGLOptions::OpenGLOptions(Application* app) : Window("OpenGL Settings", SDL_SCANCODE_UNKNOWN)
{
	App = app;
}

void OpenGLOptions::Draw()
{

	ImGui::Begin(GetName().c_str(), &active);
	{

		ImGui::Text("Tinker with the settings we are currently using...");
		ImGui::Separator();

		if (ImGui::Checkbox("Depth Testing", &App->renderer3D->depth_test))
			Check_Enable_Disable();
		ImGui::SameLine();

		if (ImGui::Checkbox("Cull Faces", &App->renderer3D->cull_face))
			Check_Enable_Disable();
		ImGui::SameLine();

		if (ImGui::Checkbox("Lighting", &App->renderer3D->lighting))
			Check_Enable_Disable();
		ImGui::SameLine();

		if (ImGui::Checkbox("Color Materials", &App->renderer3D->color_material))
			Check_Enable_Disable();

		if (ImGui::Checkbox("Textures 2D", &App->renderer3D->texture_2d))
			Check_Enable_Disable();
		ImGui::SameLine();

		ImGui::Checkbox("Render Faces", &App->renderer3D->faces);
		ImGui::SameLine();

		ImGui::Checkbox("Render Wireframe", &App->renderer3D->wireframe);
		ImGui::SameLine();

		ImGui::Checkbox("Show Normals", &App->renderer3D->render_normals);

		ImGui::SliderFloat("Normal Length", &App->renderer3D->normal_length, 0.2f, 300.0f);

		ImGui::Separator();

		if (ImGui::Button("Save Changes"))
			App->renderer3D->Save(nullptr);
	}
	ImGui::End();
}

void OpenGLOptions::Check_Enable_Disable()
{

		if (glIsEnabled(GL_DEPTH_TEST) && App->renderer3D->depth_test == false)
			glDisable(GL_DEPTH_TEST);
		else if (!glIsEnabled(GL_DEPTH_TEST) && App->renderer3D->depth_test == true)
			glEnable(GL_DEPTH_TEST);

		if (glIsEnabled(GL_CULL_FACE) && App->renderer3D->cull_face == false)
			glDisable(GL_CULL_FACE);
		else if (!glIsEnabled(GL_CULL_FACE) && App->renderer3D->cull_face == true)
			glEnable(GL_CULL_FACE);

		if (glIsEnabled(GL_LIGHTING) && App->renderer3D->lighting == false)
			glDisable(GL_LIGHTING);
		else if (!glIsEnabled(GL_LIGHTING) && App->renderer3D->lighting == true)
			glEnable(GL_LIGHTING);

		if (glIsEnabled(GL_COLOR_MATERIAL) && App->renderer3D->color_material == false)
			glDisable(GL_COLOR_MATERIAL);
		else if (!glIsEnabled(GL_COLOR_MATERIAL) && App->renderer3D->color_material == true)
			glEnable(GL_COLOR_MATERIAL);

		if (glIsEnabled(GL_TEXTURE_2D) && App->renderer3D->texture_2d == false)
			glDisable(GL_TEXTURE_2D);
		else if (!glIsEnabled(GL_TEXTURE_2D) && App->renderer3D->texture_2d == true)
			glEnable(GL_TEXTURE_2D);

}
