#include "GeoPropertiesWindow.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"

GeoPropertiesWindow::GeoPropertiesWindow() : Window("Object Properties")
{
	total_num_vertex = 0;
	total_num_faces = 0;
	check_info = false;

	selection_mask = 0;
	node_clicked = -1;
	selection_mask_checker = 0;
}

GeoPropertiesWindow::~GeoPropertiesWindow()
{
}

void GeoPropertiesWindow::Draw()
{
	ImGui::Begin(GetName().c_str(), &active);
	{
		ImGui::BeginChild("Objects List", ImVec2(250, 0), true);
		int start_val = -1;
		CreateObjLeaf(App->mesh_loader->Root_Object, 0);

		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::BeginGroup();
		{
			if (App->mesh_loader->active_objects.size() == 1) //objects.size() > 0)
			{
				ImGui::BeginChild("Object Config", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
				{
					
					ImGui::Text("%s", selected_object->name.c_str());
					ImGui::Separator();

					if (ImGui::CollapsingHeader("Object Transform"))
					{
						ImGui::Spacing();
						ImGui::Spacing();

						ImGui::Text("Use CTRL + click to input value");

						ImGui::Spacing();

						//Pos
						float pos[3] = { selected_object->transform->transform_position.x, selected_object->transform->transform_position.y, selected_object->transform->transform_position.z };
						if (ImGui::DragFloat3 ("Position", pos, 0.1f))
							selected_object->transform->SetTransformPosition(pos[0], pos[1], pos[2]);

						//Scale
						float scale[3] = { selected_object->transform->transform_scale.x, selected_object->transform->transform_scale.y, selected_object->transform->transform_scale.z };
						if (ImGui::DragFloat3("Scale", scale, 0.1f))
							selected_object->transform->SetTransformScale(scale[0], scale[1], scale[2]);

						//Rot

						float rot[3] = { selected_object->transform->transform_rotate_euler.x, selected_object->transform->transform_rotate_euler.y, selected_object->transform->transform_rotate_euler.z };
						if (ImGui::DragFloat3("Rotation", rot, 0.2f))
							selected_object->transform->SetTransformRotation((float3)rot);

						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();
					}

					if (ImGui::CollapsingHeader("Mesh Properties"))
					{
						if (check_info)
						{
							total_num_vertex = 0;
							total_num_faces = 0;

							GetTotalProperties(selected_object, total_num_vertex, total_num_faces);

							check_info = false;
						}
						ImGui::Text("Total Num. Vertices: %d", total_num_vertex);
						ImGui::Text("Total Num. Faces: %d", total_num_faces);
					}

					if (ImGui::CollapsingHeader("Texture Properties"))
					{
						if (selected_object->materials.size() > 0)
						{
							if (selected_object->materials[0]->textures.size() > 0)
							{
								for (int i = 0; i < selected_object->materials[0]->textures.size(); i++)
								{
									ImGui::Separator();

									if (check_info)
										tex_name = selected_object->materials[0]->textures[i]->filename.c_str();

									ImGui::Image(ImTextureID(selected_object->materials[0]->textures[i]->id_tex), show_size);

									if (strrchr(tex_name.c_str(), '\\') != nullptr)
										tex_name = tex_name.substr(tex_name.find_last_of("\\/") + 1);

									ImGui::TextWrapped("Texture File: %s", tex_name.c_str());

									ImGui::Text("Size: %d x %d", selected_object->materials[0]->textures[i]->width, selected_object->materials[0]->textures[i]->height);

									char str[30];
									snprintf(str, 30, "%s%d", "Use this Texture ##", i);

									if (ImGui::Button(str))
									{
										for (int j = 0; j < selected_object->meshes.size(); j++)
											selected_object->meshes[j]->Material_Ind = i;
									}

									snprintf(str, 30, "%s%d%d", "Destroy this Texture ##", i, i);
									if (ImGui::Button(str))
										selected_object->materials[0]->DestroyTexture(i);


								}
							}
						}
					}
				}
				ImGui::EndChild();

			}

			else if (App->mesh_loader->active_objects.size() > 1) //objects.size() > 0)
			{
				ImGui::Text("+ 1 obj selected");
			}

			//if (ImGui::Button("Select")) {}
			//ImGui::SameLine();
			//if (ImGui::Button("Save")) {}
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

void GeoPropertiesWindow::CreateObjLeaf(GameObject * obj, int st)
{
	
	//int num_child = 0;
	//if (obj->parent != nullptr)
	//	num_child - obj->parent->children.size();
	////num_child++;

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (obj->active ? ImGuiTreeNodeFlags_Selected : 0);
	
	if (obj->children.size() != 0)		
	{
		bool n_open = ImGui::TreeNodeEx(obj->name.c_str(), node_flags);
		if (ImGui::IsItemClicked() && ImGui::GetIO().KeyCtrl && App->mesh_loader->active_objects.size() > 0)
		{
			bool repeated = false;
			for (int i = 0; i < App->mesh_loader->active_objects.size(); i++)
			{
				if (App->mesh_loader->active_objects[i] == obj)
					repeated = true;
			}
			if (!repeated)
			{
				App->mesh_loader->active_objects.push_back(obj);
				obj->active = true;
				obj->transform->to_update = true;
			}
		}
		else if (ImGui::IsItemClicked())
		{
			App->mesh_loader->SetActiveFalse();
			App->mesh_loader->active_objects.push_back(obj);
			obj->active = true;
			obj->transform->to_update = true;

			selected_object = App->mesh_loader->active_objects[0];
			check_info = true;
		}
		if (n_open)
		{
			for (int i = 0; i < obj->children.size(); i++)
			{
				CreateObjLeaf(obj->children[i], ++st);
			}
			ImGui::TreePop();
		}
	}
	else
	{
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
		ImGui::TreeNodeEx(obj->name.c_str(), node_flags);
		if (ImGui::IsItemClicked() && ImGui::GetIO().KeyCtrl && App->mesh_loader->active_objects.size() > 0)
		{
			bool repeated = false;
			for (int i = 0; i < App->mesh_loader->active_objects.size(); i++)
			{
				if (App->mesh_loader->active_objects[i] == obj)
					repeated = true;
			}
			if (!repeated)
			{
				App->mesh_loader->active_objects.push_back(obj);
				obj->active = true;
			}
		}
		else if (ImGui::IsItemClicked())
		{
			App->mesh_loader->SetActiveFalse();
			App->mesh_loader->active_objects.push_back(obj);
			obj->active = true;

			selected_object = App->mesh_loader->active_objects[0];
			check_info = true;
		}
	}
}

void GeoPropertiesWindow::GetTotalProperties(const GameObject * obj, int &vertex, int &faces)
{
	for (int i = 0; i < obj->meshes.size(); i++)
	{
		vertex += obj->meshes[i]->num_vertex;
		faces += obj->meshes[i]->num_faces;
	}
	
	for (int i = 0; i < obj->children.size(); i++)
	{
		GetTotalProperties(obj->children[i], vertex, faces);
	}

}
