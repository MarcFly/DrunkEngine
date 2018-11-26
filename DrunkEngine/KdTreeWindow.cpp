#include "KdTreeWindow.h"
#include "KdTree.h"
#include "ConsoleWindow.h"

KDTreeWindow::KDTreeWindow() : Window("K-D Tree Properties")
{
	elements_per_node = 2;
	max_subdivisions = 10;
}

KDTreeWindow::~KDTreeWindow()
{
}

void KDTreeWindow::Draw()
{
	ImGui::Begin(GetName().c_str(), &active);
	{

		ImGui::DragInt("Max Elements per Node", &elements_per_node, 1, 1, 50);
		ImGui::DragInt("Max num of Subdivisions", &max_subdivisions, 1, 1, 50);

		if (ImGui::Button("Create K-D Tree"))
		{
			CreateKDTree();
		}

		ImGui::SameLine();

		if (ImGui::Button("Destroy K-D Tree"))
				App->gameObj->DeleteSceneKDTree();

		ImGui::SameLine();

		if (App->gameObj->GetSceneKDTree() != nullptr)
			ImGui::Checkbox("Render K-D Tree", &App->gameObj->GetSceneKDTree()->to_draw);

	}
	ImGui::End();
}

void KDTreeWindow::CreateKDTree()
{
	App->gameObj->DeleteSceneKDTree();

	if (App->gameObj->static_objects_in_scene.size() > 0)
	{
		App->gameObj->SetSceneKDTree(elements_per_node, max_subdivisions);
		App->ui->console_win->AddLog("New KD-Tree created with %i nodes.", App->gameObj->GetSceneKDTree()->nodes.size());
	}
}
