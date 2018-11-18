#include "SceneViewerWindow.h"
#include "GameObject.h"

SceneViewer::SceneViewer() : Window("Scene Viewer")
{
	selection_mask = 0;
	node_clicked = -1;
	selection_mask_checker = 0;
}

SceneViewer::~SceneViewer()
{
}

void SceneViewer::Draw()
{
	ImGui::Begin(GetName().c_str(), &active);
	{
		CreateObjLeaf(App->gameObj->getRootObj(), 0);
	}
	ImGui::End();
}

void SceneViewer::CreateObjLeaf(GameObject * obj, int st)
{
	
	//int num_child = 0;
	//if (obj->parent != nullptr)
	//	num_child - obj->parent->children.size();
	////num_child++;

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (obj->active ? ImGuiTreeNodeFlags_Selected : 0);
	
	if (obj->children.size() != 0)		
	{
		bool n_open = ImGui::TreeNodeEx(obj->name.c_str(), node_flags);
		if (ImGui::IsItemClicked() && ImGui::GetIO().KeyCtrl && App->gameObj->active_objects.size() > 0)
		{
			bool repeated = false;
			for (int i = 0; i < App->gameObj->active_objects.size(); i++)
			{
				if (App->gameObj->active_objects[i] == obj)
					repeated = true;
			}
			if (!repeated)
			{
				App->gameObj->active_objects.push_back(obj);
				obj->active = true;
				obj->GetTransform()->update_bounding_box = true;
			}
		}
		else if (ImGui::IsItemClicked())
		{
			App->gameObj->SetActiveFalse();
			App->gameObj->active_objects.push_back(obj);
			obj->active = true;
			obj->GetTransform()->update_bounding_box = true;

			selected_object = App->gameObj->active_objects[0];
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
		if (ImGui::IsItemClicked() && ImGui::GetIO().KeyCtrl && App->gameObj->active_objects.size() > 0)
		{
			bool repeated = false;
			for (int i = 0; i < App->gameObj->active_objects.size(); i++)
			{
				if (App->gameObj->active_objects[i] == obj)
					repeated = true;
			}
			if (!repeated)
			{
				App->gameObj->active_objects.push_back(obj);
				obj->active = true;
			}
		}
		else if (ImGui::IsItemClicked())
		{
			App->gameObj->SetActiveFalse();
			App->gameObj->active_objects.push_back(obj);
			obj->active = true;

			selected_object = App->gameObj->active_objects[0];
			check_info = true;
		}
	}
}