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

	if (obj != nullptr)
	{
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (obj->sv_active? ImGuiTreeNodeFlags_Selected : 0);
		std::string LeafID = obj->name + "##" + std::to_string(st);

		if (obj->children.size() != 0)
		{
			bool n_open = ImGui::TreeNodeEx(LeafID.c_str(), node_flags);
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
					obj->sv_active= true;
				}
			}
			else if (ImGui::IsItemClicked())
			{
				App->gameObj->SetActiveFalse();
				App->gameObj->active_objects.push_back(obj);
				obj->sv_active= true;

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
			ImGui::TreeNodeEx(LeafID.c_str(), node_flags);
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
					obj->sv_active= true;
				}
			}
			else if (ImGui::IsItemClicked())
			{
				App->gameObj->SetActiveFalse();
				App->gameObj->active_objects.push_back(obj);
				obj->sv_active= true;

				selected_object = App->gameObj->active_objects[0];
				check_info = true;
			}
		}
	}
}