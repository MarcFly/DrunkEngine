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

					if (selected_object->parent != nullptr)
					{
						ImGui::Separator();

						if (ImGui::CollapsingHeader("Object Transform"))
						{
							ImGui::Spacing();

							if (!selected_object->parent->is_static)
							{
								if (ImGui::Checkbox("Static", &selected_object->is_static))
									selected_object->RecursiveSetStatic(selected_object, selected_object->is_static);
							}
							else
								ImGui::Text("Parent is static, you cannot modify this object");

							ImGui::Spacing();

							ImGui::Text("Use CTRL + click to input value");

							ImGui::Spacing();

							//Pos
							float pos[3] = { selected_object->GetTransform()->position.x, selected_object->GetTransform()->position.y, selected_object->GetTransform()->position.z };
							if (!selected_object->is_static && ImGui::DragFloat3("Position", pos, 0.1f))
								selected_object->GetTransform()->SetTransformPosition(pos[0], pos[1], pos[2]);

							//Scale
							float scale[3] = { selected_object->GetTransform()->scale.x, selected_object->GetTransform()->scale.y, selected_object->GetTransform()->scale.z };
							if (!selected_object->is_static && ImGui::DragFloat3("Scale", scale, 0.1f))
								selected_object->GetTransform()->SetTransformScale(scale[0], scale[1], scale[2]);

							//Rot
							float rot[3] = { selected_object->GetTransform()->rotate_euler.x, selected_object->GetTransform()->rotate_euler.y, selected_object->GetTransform()->rotate_euler.z };
							if (!selected_object->is_static && ImGui::DragFloat3("Rotation", rot, 0.2f))
								selected_object->GetTransform()->SetTransformRotation((float3)rot);

							ImGui::Spacing();
						}

						if (ImGui::CollapsingHeader("Mesh Properties"))
						{
							if (check_info)
							{
								std::vector<uint> props = selected_object->GetMeshProps();

								total_num_vertex = props[0];
								total_num_faces = props[1];
								check_info = false;
							}
							ImGui::Text("Total Num. Vertices: %d", total_num_vertex);
							ImGui::Text("Total Num. Faces: %d", total_num_faces);
						}

						if (ImGui::CollapsingHeader("Texture Properties"))
						{
							std::vector<Component*> cmp_mats;
							cmp_mats = selected_object->GetComponents(CT_Material);

							for (int i = 0; cmp_mats[0] != nullptr && i < cmp_mats[0]->AsMaterial()->textures.size(); i++)
							{
								Texture* aux = cmp_mats[0]->AsMaterial()->textures[i];
								ImGui::Separator();

								if (check_info)
									tex_name = aux->filename.c_str();

								ImGui::Image(ImTextureID(aux->id_tex), show_size);

								if (strrchr(tex_name.c_str(), '\\') != nullptr)
									tex_name = tex_name.substr(tex_name.find_last_of("\\/") + 1);

								ImGui::TextWrapped("Texture File: %s", tex_name.c_str());

								ImGui::Text("Size: %d x %d", aux->width, aux->height);

								char str[30];
								snprintf(str, 30, "%s%d", "Use this Texture ##", i);

								if (ImGui::Button(str))
								{
									std::vector<Component*> cmp_meshes;
									cmp_meshes = selected_object->GetComponents(CT_Mesh);
									for (int j = 0; j < cmp_meshes.size(); j++)
										cmp_meshes[i]->AsMesh()->Material_Ind = i;
								}

								snprintf(str, 30, "%s%d%d", "Destroy this Texture ##", i, i);
								if (ImGui::Button(str))
									cmp_mats[0]->AsMaterial()->DestroyTexture(i);


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
				obj->GetTransform()->to_update = true;
			}
		}
		else if (ImGui::IsItemClicked())
		{
			App->mesh_loader->SetActiveFalse();
			App->mesh_loader->active_objects.push_back(obj);
			obj->active = true;
			obj->GetTransform()->to_update = true;

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
