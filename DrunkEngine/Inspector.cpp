#include "Inspector.h"
#include "GameObject.h"

Inspector::Inspector() : Window("Inspector")
{
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
		CreateObjLeaf(App->scene->Root_Object, 0);
	}
	ImGui::End();
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
				obj->GetTransform()->update_bouding_box = true;
			}
		}
		else if (ImGui::IsItemClicked())
		{
			App->scene->SetActiveFalse();
			App->scene->active_objects.push_back(obj);
			obj->active = true;
			obj->GetTransform()->update_bouding_box = true;

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