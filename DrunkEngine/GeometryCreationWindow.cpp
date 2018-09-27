#include "GeometryCreationWindow.h"
#include "ModulePhysics3D.h"

GEOMWindow::GEOMWindow(Application* app) : Window("Create Objects", SDL_SCANCODE_H)
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
		ImGui::SliderFloat3("Center", &sphere_center[0], 0, 10);
		ImGui::SliderFloat("Radius", &sphere_radius, 0, 10);
		ImGui::Checkbox("PhysObj", &sphere_phys);
		ImGui::SameLine();
		if(ImGui::Button("Create"))
		{
			if (sphere_phys)
				App->physics->AddBody(sphere_center, PSphere(sphere_radius));
			else
				App->physics->AddSphereMath(sphere_center, sphere_radius);
		}

		ImGui::Separator();
	}
	ImGui::End();
}