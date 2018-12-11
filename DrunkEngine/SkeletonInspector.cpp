#include "SkeletonInspector.h"
#include "ResourceSkeleton.h"
#include "ComponentSkeleton.h"

SkeletonInspectorWin::SkeletonInspectorWin() : Window("Skeleton Inspector")
{
	base_skel = nullptr;
	last_weight = nullptr;
	check_info = false;
	selected_bone = nullptr;
}

SkeletonInspectorWin::~SkeletonInspectorWin()
{
	base_skel = nullptr;
	last_weight = nullptr;
	selected_bone = nullptr;
	
	active_bones.clear();
	selection_mas_vec.clear();

}

void SkeletonInspectorWin::Draw()
{
	if (active)
	{
		ImGui::Begin(GetName().c_str(), &active);
		{
			if (base_skel != nullptr)
			{
				ImGui::BeginChild("Bone List", ImVec2(250, 0), true);
				CreateBoneLeaf(base_skel->r_skel->bones[0], 0);
				ImGui::EndChild();

				ImGui::SameLine();
				ImGui::BeginGroup();
				{
					if (selection_mask != 0)
					{
						ImGui::BeginChild("Bone Data"), ImVec2(0, -ImGui::GetFrameHeightWithSpacing());
						{
							ShowBoneData();
						}
						ImGui::EndChild();
					}
				}
				ImGui::EndGroup();
			}
		}
		ImGui::End();
	}
}

void SkeletonInspectorWin::SetActiveFalse()
{
	for (int i = 0; i < active_bones.size(); i++)
	{
		active_bones[i]->active = false;
	}
	active_bones.clear();
}

void SkeletonInspectorWin::CreateBoneLeaf(Bone* bone, int st)
{
	if (bone != nullptr)
	{
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (bone->active ? ImGuiTreeNodeFlags_Selected : 0);
		std::string LeafID = bone->name + "##" + std::to_string(st);

		if (bone->children.size() != 0)
		{
			bool n_open = ImGui::TreeNodeEx(LeafID.c_str(), node_flags);
			if (ImGui::IsItemClicked() && ImGui::GetIO().KeyCtrl && App->gameObj->active_objects.size() > 0)
			{
				bool repeated = false;
				for (int i = 0; i < active_bones.size(); i++)
				{
					if (active_bones[i] == bone)
						repeated = true;
				}
				if (!repeated)
				{
					active_bones.push_back(bone);
					bone->active = true;
				}
			}
			else if (ImGui::IsItemClicked())
			{
				SetActiveFalse();
				active_bones.push_back(bone);
				bone->active = true;

				selected_bone = active_bones[0];
				check_info = true;
			}
			if (n_open)
			{
				for (int i = 0; i < bone->children.size(); i++)
				{
					CreateBoneLeaf(bone->children[i], ++st);
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
					if (active_bones[i] == bone)
						repeated = true;
				}
				if (!repeated)
				{
					active_bones.push_back(bone);
					bone->active = true;
				}
			}
			else if (ImGui::IsItemClicked())
			{
				SetActiveFalse();
				active_bones.push_back(bone);
				bone->active = true;

				selected_bone = active_bones[0];
				check_info = true;
			}
		}
	}
}

void SkeletonInspectorWin::ShowBoneData()
{
	if (selected_bone != nullptr)
	{
		if (check_info)
		{
			for (int j = 0; j < last_bone_weights.size(); j++)
				delete last_bone_weights[j];
			last_bone_weights.clear();
			for (int i = 0; i < selected_bone->weights.size(); i++)
			{
				char* test;
				std::string cpy = std::to_string(selected_bone->weights[i]->VertexID) + "\0";
				test = new char[cpy.length() + 1];
				char* cursor = test;
				memcpy(cursor, cpy.c_str(), cpy.length());
				cursor += cpy.length();
				memcpy(cursor, "\0", 1);

				last_bone_weights.push_back(test);
			}
			check_info = false;
		}
		ImGui::Text("Weight Amount: %i", selected_bone->weights.size());

		ImGui::Indent(25);
		if (last_bone_weights.size() > 0)
		{
			if (ImGui::Combo("##Weight", &weight_ind, &last_bone_weights[0], last_bone_weights.size()))
				last_weight = selected_bone->weights[weight_ind];
			if (last_weight != nullptr)
				ImGui::SliderFloat("Weight Value", &last_weight->w, 0, 1);
		}
		ImGui::Indent(-25);
	}
}