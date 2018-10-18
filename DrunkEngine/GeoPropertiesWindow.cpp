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

		if (node_clicked != -1)
		{
			if (ImGui::GetIO().KeyCtrl)
				selection_mask ^= (1 << node_clicked);
			else
				selection_mask = (1 << node_clicked);
		}

		node_clicked = -1;

		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::BeginGroup();
		{
			if (selection_mask != 0) //objects.size() > 0)
			{
				ImGui::BeginChild("Object Config", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
				{

					if (selection_mask_checker != selection_mask)
					{
						selection_mask_checker = selection_mask;
						selected_object = App->mesh_loader->GetSelected(App->mesh_loader->Root_Object);
						check_info = true;
					}
					
					ImGui::Text("%s", selected_object->name.c_str());
					ImGui::Separator();

					if (ImGui::CollapsingHeader("Object Transform"))
					{
						ImGui::Text("Transform Position:   x:%.02f   y:%.02f   z:%.02f", selected_object->transform->transform_position.x, selected_object->transform->transform_position.y, selected_object->transform->transform_position.z);
						ImGui::Text("Transform Scale:      x:%.02f   y:%.02f   z:%.02f", selected_object->transform->transform_scale.x, selected_object->transform->transform_scale.y, selected_object->transform->transform_scale.z);
						ImGui::Text("Transform Rotation:   x:%.02f   y:%.02f   z:%.02f\n\n", RadToDeg(selected_object->transform->transform_rotate.GetEuler().x), RadToDeg(selected_object->transform->transform_rotate.GetEuler().y), RadToDeg(selected_object->transform->transform_rotate.GetEuler().z));
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
			//if (ImGui::Button("Select")) {}
			//ImGui::SameLine();
			//if (ImGui::Button("Save")) {}
		}
		ImGui::EndGroup();

	}
	ImGui::End();

	/*ImGui::Begin(GetName().c_str(), &active);
	{
		// left

		selected = 0;

		ImGui::BeginChild("left pane", ImVec2(250, 0), true);
		//for (int i = 0; i < 2; i++)
		//{
		//	char label[128];
		//	sprintf(label, "MyObject %d", i);
		//	if (ImGui::Selectable(label, selected == i))
		//		selected = i;
		//}

		std::vector<GameObject> objects = App->mesh_loader->getObjects();

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
						if (objects[selected].materials.size() > 0)
						{
							for (int i = 0; i < objects[selected].materials.size(); i++)
							{
								ImGui::Separator();

								if (check_info)
									tex_name = objects[selected].materials[i].filename.c_str();

								ImGui::Image(ImTextureID(objects[selected].materials[i].id_tex), show_size);

								if (strrchr(tex_name.c_str(), '\\') != nullptr)
									tex_name = tex_name.substr(tex_name.find_last_of("\\/") + 1);

								ImGui::TextWrapped("Texture File: %s", tex_name.c_str());

								ImGui::Text("Size: %d x %d", objects[selected].materials[i].width, objects[selected].materials[i].height);

								char str[30];
								snprintf(str, 30, "%s%d", "Use this Texture ##", i);

								if (ImGui::Button(str))
								{
									for (int j = 0; j < objects[selected].meshes.size(); j++)
										App->mesh_loader->Objects[selected].meshes[j].tex_index = i;
								}

								snprintf(str, 30, "%s%d%d", "Destroy this Texture ##", i, i);
								if (ImGui::Button(str))
									App->mesh_loader->DestroyTexture(&App->mesh_loader->Objects[selected], i);
							}
						}
					}
				}
				ImGui::EndChild();

			}
			//if (ImGui::Button("Select")) {}
			//ImGui::SameLine();
			//if (ImGui::Button("Save")) {}
		}
		ImGui::EndGroup();
	}
	ImGui::End();*/
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

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask == (1 << st)) ? ImGuiTreeNodeFlags_Selected : 0);
	
	if (obj->children.size() != 0)		
	{
		bool n_open = ImGui::TreeNodeEx(obj->name.c_str(), node_flags);
		if (ImGui::IsItemClicked())
		{
			node_clicked = st;
			App->mesh_loader->SetSelectedFalse(App->mesh_loader->Root_Object);
			obj->selected = true;
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
		if (ImGui::IsItemClicked())
		{
			node_clicked = st;
			App->mesh_loader->SetSelectedFalse(App->mesh_loader->Root_Object);
			obj->selected = true;
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
