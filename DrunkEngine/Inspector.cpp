#include "Inspector.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "Octree.h"

Inspector::Inspector() : Window("Object Properties")
{
	total_num_vertex = 0;
	total_num_faces = 0;
	check_info = false;

	selection_mask = 0;
	node_clicked = -1;
	selection_mask_checker = 0;
}

Inspector::~Inspector()
{
}

void Inspector::Draw()
{
	ImGui::Begin(GetName().c_str(), &active);
	{
		ImGui::BeginChild("Objects List", ImVec2(250, 0), true);

		CreateObjLeaf(App->scene->Root_Object, 0);

		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::BeginGroup();
		{
			if (App->scene->active_objects.size() == 1) //objects.size() > 0)
			{
				ImGui::BeginChild("Object Config", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
				{
					
					ImGui::Text("%s", selected_object->name.c_str());

					if (selected_object->parent != nullptr)
					{
						for (int i = 0; i < selected_object->components.size(); i++)
						{
							ComponentInspector(selected_object->components[i]);
						}
						
					}
				}
				ImGui::EndChild();

			}

			else if (App->scene->active_objects.size() > 1) //objects.size() > 0)
			{
				ImGui::Text("+ 1 obj selected");
			}

			if (ImGui::CollapsingHeader("Octree"))
			{
				if (ImGui::Button("Create Octree"))
				{
					if (App->scene->Root_Object->Scene_Octree != nullptr)
						App->scene->Root_Object->Scene_Octree->CleanUp();

					App->scene->Root_Object->Scene_Octree = new Octree(2);
				}

				ImGui::SameLine();

				if (ImGui::Button("Delete Octree"))
				{
					if (App->scene->Root_Object->Scene_Octree != nullptr)
						App->scene->Root_Object->Scene_Octree->CleanUp();
				}

			}

			//if (ImGui::Button("Select")) {}
			//ImGui::SameLine();
			//if (ImGui::Button("Save")) {}
		}
		ImGui::EndGroup();
	}
	ImGui::End();
}

void Inspector::CheckMeshInfo()
{
	check_info = true;
	total_num_vertex = 0;
	total_num_faces = 0;
}

void Inspector::CreateObjLeaf(GameObject * obj, int st)
{
	
	//int num_child = 0;
	//if (obj->parent != nullptr)
	//	num_child - obj->parent->children.size();
	////num_child++;

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (obj->active ? ImGuiTreeNodeFlags_Selected : 0);
	
	if (obj->children.size() != 0)		
	{
		bool n_open = ImGui::TreeNodeEx(obj->name.c_str(), node_flags);
		if (ImGui::IsItemClicked() && ImGui::GetIO().KeyCtrl && App->scene->active_objects.size() > 0)
		{
			bool repeated = false;
			for (int i = 0; i < App->scene->active_objects.size(); i++)
			{
				if (App->scene->active_objects[i] == obj)
					repeated = true;
			}
			if (!repeated)
			{
				App->scene->active_objects.push_back(obj);
				obj->active = true;
				obj->GetTransform()->to_update = true;
			}
		}
		else if (ImGui::IsItemClicked())
		{
			App->scene->SetActiveFalse();
			App->scene->active_objects.push_back(obj);
			obj->active = true;
			obj->GetTransform()->to_update = true;

			selected_object = App->scene->active_objects[0];
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
		if (ImGui::IsItemClicked() && ImGui::GetIO().KeyCtrl && App->scene->active_objects.size() > 0)
		{
			bool repeated = false;
			for (int i = 0; i < App->scene->active_objects.size(); i++)
			{
				if (App->scene->active_objects[i] == obj)
					repeated = true;
			}
			if (!repeated)
			{
				App->scene->active_objects.push_back(obj);
				obj->active = true;
			}
		}
		else if (ImGui::IsItemClicked())
		{
			App->scene->SetActiveFalse();
			App->scene->active_objects.push_back(obj);
			obj->active = true;

			selected_object = App->scene->active_objects[0];
			check_info = true;
		}
	}
}

//----------------------------
// Component Inspectors
void Inspector::ComponentInspector(Component* component)
{
	switch (component->type)
	{
	case CT_Mesh: MeshInspector(component->AsMesh()); break;
	case CT_Material: MatInspector(component->AsMaterial()); break;
	case CT_Camera: CamInspector(component->AsCamera()); break;
	case CT_Transform: TransformInspector(component->AsTransform()); break;
	}
}

void Inspector::MeshInspector(ComponentMesh* mesh)
{
	if (ImGui::CollapsingHeader("Mesh"))
	{
		if (check_info)
		{
			std::vector<uint> props = mesh->parent->GetMeshProps();

			total_num_vertex = props[0];
			total_num_faces = props[1];
			check_info = false;
		}
		ImGui::Text("Total Num. Vertices: %d", total_num_vertex);
		ImGui::Text("Total Num. Faces: %d", total_num_faces);
	}
}

void Inspector::MatInspector(ComponentMaterial* mat)
{
	if (ImGui::CollapsingHeader("Material"))
	{

		for (int i = 0; mat != nullptr && i < mat->textures.size(); i++)
		{
			Texture* aux = mat->textures[i];
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
				cmp_meshes = mat->parent->GetComponents(CT_Mesh);
				for (int j = 0; j < cmp_meshes.size(); j++)
					cmp_meshes[i]->AsMesh()->Material_Ind = i;
			}

			snprintf(str, 30, "%s%d%d", "Destroy this Texture ##", i, i);
			if (ImGui::Button(str))
				mat->DestroyTexture(i);


		}


	}
}

void Inspector::CamInspector(ComponentCamera* cam)
{

}

void Inspector::TransformInspector(ComponentTransform* transform)
{
	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGui::Spacing();

		if (!selected_object->parent->is_static)
		{
			if (ImGui::Checkbox("Static", &transform->parent->is_static))
				selected_object->RecursiveSetStatic(transform->parent, transform->parent->is_static);
		}
		else
			ImGui::Text("Parent is static, you cannot modify this object");

		ImGui::Spacing();

		ImGui::Text("Use CTRL + click to input value");

		ImGui::Spacing();

		//Pos
		float pos[3] = { transform->position.x, transform->position.y, transform->position.z };
		if (!transform->parent->is_static && ImGui::DragFloat3("Position", pos, 0.1f))
			transform->SetTransformPosition(pos[0], pos[1], pos[2]);

		//Scale
		float scale[3] = { transform->scale.x, transform->scale.y, transform->scale.z };
		if (!transform->parent->is_static && ImGui::DragFloat3("Scale", scale, 0.1f))
			transform->SetTransformScale(scale[0], scale[1], scale[2]);

		//Rot
		float rot[3] = { transform->rotate_euler.x, transform->rotate_euler.y, transform->rotate_euler.z };
		if (!transform->parent->is_static && ImGui::DragFloat3("Rotation", rot, 0.2f))
			transform->SetTransformRotation((float3)rot);

		ImGui::Spacing();
	}
}