#include "GeometryCreationWindow.h"
#include "ModulePhysics3D.h"

GEOMWindow::GEOMWindow(Application* app) : Window("Create Objects", SDL_SCANCODE_UNKNOWN)
{
	App = app;
}

GEOMWindow::~GEOMWindow() {}

void GEOMWindow::Draw()
{
	ImGui::Begin(GetName().c_str(), &active);
	{
		ImGui::Text("Create Various Objects, mathematically or with physics properties (if available)");
		
		ImGui::Separator();

		ImGui::Text("Sphere");
		ImGui::SliderFloat3("Sphere Center", &sphere_center[0], 0, 10);
		ImGui::SliderFloat("Sphere Radius", &sphere_radius, 0, 10);
		ImGui::Checkbox("Sphere PhysObj", &sphere_phys);
		ImGui::SameLine();
		if(ImGui::Button("Create Sphere"))
		{
			App->physics->AddBody(sphere_center, PSphere(sphere_radius), sphere_phys);
		}

		ImGui::Separator();

		ImGui::Text("Cube");
		ImGui::SliderFloat3("Cube Center", &box_center[0], 0, 10);
		ImGui::SliderFloat("Cube Size", &box_size, 0, 10);
		ImGui::Checkbox("Cube PhysObj", &box_phys);
		ImGui::SameLine();
		if (ImGui::Button("Create Cube"))
		{
			App->physics->AddBody(box_center, PCube(box_size), box_phys);
		}

	}
	ImGui::End();
}