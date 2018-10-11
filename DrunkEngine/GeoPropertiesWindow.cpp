#include "GeoPropertiesWindow.h"
#include "ModuleManageMesh.h"

GeoPropertiesWindow::GeoPropertiesWindow(Application * app) : Window("Object Properties")
{
	App = app;
	num_vertex = 0;
	num_faces = 0;
	check_info = false;
}

GeoPropertiesWindow::~GeoPropertiesWindow()
{
}

void GeoPropertiesWindow::Draw()
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

		if (objects.size() > 0)
		{
			for (int i = 0; i < objects.size(); i++)
			{
				char label[128];
				sprintf(label, "%s", objects[i].name.c_str());
				if (ImGui::Selectable(label, selected == i))
				{
					selected = i;
					check_info = true;

					num_vertex = 0;
					num_faces = 0;
				}
			}
		}
		ImGui::EndChild();
		ImGui::SameLine();

		// right
		ImGui::BeginGroup();
		{
			if (objects.size() > 0)
			{
				ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
				{
					ImGui::Text("%s", objects[selected].name.c_str());
					ImGui::Separator();

					if (ImGui::CollapsingHeader("Mesh Transform"))
					{
						//ImGui::Text("Position: x:%d y:%d z:%d", );
						//ImGui::Text("Rotation: x:%d y:%d z:%d", );
						//ImGui::Text("Scale: x:%d y:%d z:%d", );
					}

					if (ImGui::CollapsingHeader("Mesh Properties"))
					{	
						if (check_info)
						{
							for (int i = 0; i < objects[selected].meshes.size(); i++)
							{
								num_vertex += objects[selected].meshes[i].num_vertex;
								num_faces += objects[selected].meshes[i].num_faces;
							}
							check_info = false;
						}
						ImGui::Text("Num. Vertices: %d", num_vertex);
						ImGui::Text("Num. Faces: %d", num_faces);
					}

					if (ImGui::CollapsingHeader("Texture Properties"))
					{

					}
				}
				ImGui::EndChild();

			}
			if (ImGui::Button("Select")) {}
			ImGui::SameLine();
			if (ImGui::Button("Save")) {}
		}
		ImGui::EndGroup();
	}
	ImGui::End();
}

void GeoPropertiesWindow::CheckMeshInfo()
{
	check_info = true;
	num_vertex = 0;
	num_faces = 0;
}
