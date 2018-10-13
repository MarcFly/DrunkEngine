#include "GeoPropertiesWindow.h"

GeoPropertiesWindow::GeoPropertiesWindow() : Window("Object Properties")
{
	total_num_vertex = 0;
	total_num_faces = 0;
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
				sprintf(label, "%s (%d meshes)", objects[i].name.c_str(), objects[i].meshes.size());
				if (ImGui::Selectable(label, selected == i))
				{
					selected = i;
					check_info = true;

					total_num_vertex = 0;
					total_num_faces = 0;
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
						for (int i = 0; i < objects[selected].meshes.size(); i++)
						{
							ImGui::Text("%s", objects[selected].meshes[i].name.c_str());
							ImGui::Text("Transform Position:   x:%.02f   y:%.02f   z:%.02f", objects[selected].meshes[i].transform_position.x, objects[selected].meshes[i].transform_position.y, objects[selected].meshes[i].transform_position.z);
							ImGui::Text("Transform Scale:      x:%.02f   y:%.02f   z:%.02f", objects[selected].meshes[i].transform_scale.x, objects[selected].meshes[i].transform_scale.y, objects[selected].meshes[i].transform_scale.z);
							ImGui::Text("Transform Rotation:   x:%.02f   y:%.02f   z:%.02f\n\n", RadToDeg(objects[selected].meshes[i].transform_rotate.x), RadToDeg(objects[selected].meshes[i].transform_rotate.y), RadToDeg(objects[selected].meshes[i].transform_rotate.z));
						}
					}

					if (ImGui::CollapsingHeader("Mesh Properties"))
					{	
						if (check_info)
						{
							for (int i = 0; i < objects[selected].meshes.size(); i++)
							{
								total_num_vertex += objects[selected].meshes[i].num_vertex;
								total_num_faces += objects[selected].meshes[i].num_faces;
							}
							check_info = false;
						}
						ImGui::Text("Total Num. Vertices: %d", total_num_vertex);
						ImGui::Text("Total Num. Faces: %d", total_num_faces);
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
	total_num_vertex = 0;
	total_num_faces = 0;
}
