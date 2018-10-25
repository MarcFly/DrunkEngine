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
					ImGui::Separator();

					if (ImGui::CollapsingHeader("Object Transform"))
					{
						ImGui::Spacing();
						ImGui::Spacing();

						ImGui::Text("Use CTRL + click to input value");

						ImGui::Spacing();

						ComponentTransform* aux = selected_object->GetComponent(CT_Transform)->AsTransform();
						//Pos
						if (aux != nullptr)
						{
							float pos[3] = { aux->position.x, aux->position.y, aux->position.z };
							if (ImGui::DragFloat3("Position", pos, 1.f))
								aux->SetTransformPosition(pos[0], pos[1], pos[2]);

							//Scale
							float scale[3] = { aux->scale.x, aux->scale.y, aux->scale.z };
							if (ImGui::DragFloat3("Scale", scale, 0.1f))
								aux->SetTransformScale(scale[0], scale[1], scale[2]);

							//Rot

							float rot[3] = { aux->rotate_euler.x, aux->rotate_euler.y, aux->rotate_euler.z };
							if (ImGui::DragFloat3("Rotation", rot, 1.f))
								aux->SetTransformRotation((float3)rot);
						}
						ImGui::Spacing();
						ImGui::Spacing();
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
							ImGui::Text("Transform Position:   x:%.02f   y:%.02f   z:%.02f", objects[selected].meshes[i].position.x, objects[selected].meshes[i].position.y, objects[selected].meshes[i].position.z);
							ImGui::Text("Transform Scale:      x:%.02f   y:%.02f   z:%.02f", objects[selected].meshes[i].scale.x, objects[selected].meshes[i].scale.y, objects[selected].meshes[i].scale.z);
							ImGui::Text("Transform Rotation:   x:%.02f   y:%.02f   z:%.02f\n\n", RadToDeg(objects[selected].meshes[i].rotate.x), RadToDeg(objects[selected].meshes[i].rotate.y), RadToDeg(objects[selected].meshes[i].rotate.z));
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
