#include "KdTreeWindow.h"
#include "KdTree.h"

KDTreeWindow::KDTreeWindow() : Window("K-D Tree Properties")
{
	elements_per_node = 1;
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
			if (App->gameObj->GetSceneKDTree() != nullptr)
				App->gameObj->GetSceneKDTree()->CleanUp();

			App->gameObj->SetSceneKDTree(elements_per_node, max_subdivisions);
		}

		ImGui::SameLine();

		if (ImGui::Button("Destroy K-D Tree"))
		{
			if (App->gameObj->GetSceneKDTree() != nullptr)
				App->gameObj->GetSceneKDTree()->CleanUp();
		}		
	}
	ImGui::End();
}
