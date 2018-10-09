#include "GeoTransformWindow.h"
#include "ModuleManageMesh.h"

GeoTransformWindow::GeoTransformWindow(Application * app) : Window("Transform objects", SDL_SCANCODE_UNKNOWN)
{
	App = app;
}

GeoTransformWindow::~GeoTransformWindow()
{
}

void GeoTransformWindow::Draw()
{
	ImGui::Begin(GetName().c_str(), &active);
	{
		// left
		static int selected = 0;
		ImGui::BeginChild("left pane", ImVec2(250, 0), true);
		//for (int i = 0; i < 2; i++)
		//{
		//	char label[128];
		//	sprintf(label, "MyObject %d", i);
		//	if (ImGui::Selectable(label, selected == i))
		//		selected = i;
		//}

		std::vector<obj_data> objects = App->mesh_loader->getObjects();

		for (int i = 0; i < objects.size(); i++)
		{
			char label[128];
			sprintf(label, "%s", objects[i].name.c_str());
			if (ImGui::Selectable(label, selected == i))
				selected = i;
		}

		ImGui::EndChild();
		ImGui::SameLine();

		// right
		ImGui::BeginGroup();
		{
			ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
			{
				ImGui::Text("%s", objects[selected].name.c_str());
				ImGui::Separator();

			}
			ImGui::EndChild();

			if (ImGui::Button("Select")) {}
			ImGui::SameLine();
			if (ImGui::Button("Save")) {}
		}
		ImGui::EndGroup();
	}
	ImGui::End();
}
