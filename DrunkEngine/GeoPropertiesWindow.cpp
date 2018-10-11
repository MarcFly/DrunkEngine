#include "GeoPropertiesWindow.h"

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

					if (ImGui::CollapsingHeader("Object Transform"))
					{
						ImGui::Text("Transform Position: x:%d y:%d z:%d", objects[selected].transform_position.x, objects[selected].transform_position.y, objects[selected].transform_position.z);
							ImGui::Text("Transform Rotate: x:%d y:%d z:%d w:%d", objects[selected].transform_rotate.x, objects[selected].transform_rotate.y, objects[selected].transform_rotate.z, objects[selected].transform_rotate.w);
							ImGui::Text("Transform Scale: x:%d y:%d z:%d", objects[selected].transform_scale.x, objects[selected].transform_scale.y, objects[selected].transform_scale.z);
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
						if (objects[selected].textures.size() > 0)
						{
							ImGui::Separator();

							if (check_info)
								tex_name = objects[selected].textures[objects[selected].textures.size() - 1].filename.c_str();

							ImGui::Image(ImTextureID(objects[selected].textures[objects[selected].textures.size() - 1].id_tex), show_size);

							if (strrchr(tex_name.c_str(), '\\') != nullptr)
								tex_name = tex_name.substr(tex_name.find_last_of("\\/") + 1);

							ImGui::TextWrapped("Texture File: %s", tex_name.c_str());

							ImGui::Text("Size: %d x %d", objects[selected].textures[objects[selected].textures.size() - 1].width, objects[selected].textures[objects[selected].textures.size() - 1].height);
						}
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
