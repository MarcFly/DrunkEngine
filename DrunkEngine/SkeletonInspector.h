#ifndef _SKELETON_INSPECTOR_
#define _SKELETON_INSPECTOR_

#include "Window.h"
#include "ModuleScene.h"

class ComponentSkeleton;
struct Bone;
struct BoneWeight;

class SkeletonInspectorWin : public Window
{
public:
	SkeletonInspectorWin();
	virtual ~SkeletonInspectorWin();

	void Draw() override;

	void CreateBoneLeaf(Bone* bone, int st);

	void SetActiveFalse();
	void ShowBoneData();

public:

	std::vector<const char*> last_bone_weights;

	ComponentSkeleton* base_skel;
	BoneWeight* last_weight;

	int weight_ind = 0;

	bool check_info;
	bool selected_mask;

	int selection_mask;
	int node_clicked;

	int selection_mask_checker;

	Bone* selected_bone = nullptr;
	std::vector<Bone*> active_bones;
	std::vector<int> selection_mas_vec;

	ImVec2 show_size = { 128,128 };
};

#endif