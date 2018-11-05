#include "OctreeWindow.h"
#include "Octree.h"

OctreeWindow::OctreeWindow() : Window("Octree Properties")
{
	elements_per_node = 1;
	max_subdivisions = 10;
}

OctreeWindow::~OctreeWindow()
{
}

void OctreeWindow::Draw()
{
	ImGui::Begin(GetName().c_str(), &active);
	{

		ImGui::DragInt("Max Elements per Node", &elements_per_node, 1, 1, 50);
		ImGui::DragInt("Max num of Subdivisions", &max_subdivisions, 1, 1, 50);

		if (ImGui::Button("Create Octree"))
		{
			if (App->scene->Root_Object->Scene_Octree != nullptr)
				App->scene->Root_Object->Scene_Octree->CleanUp();

			App->scene->Root_Object->Scene_Octree = new Octree(elements_per_node, max_subdivisions);
		}

		ImGui::SameLine();

		if (ImGui::Button("Destroy Octree"))
		{
			if (App->scene->Root_Object->Scene_Octree != nullptr)
				App->scene->Root_Object->Scene_Octree->CleanUp();
		}		
	}
	ImGui::End();
}
