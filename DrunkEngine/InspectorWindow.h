#ifndef _INSPECTOR_WINDOW_
#define _INSPECTOR_WINDOW_

#include "Window.h"
#include "ModuleScene.h"

class GameObject;
struct Bone;
struct BoneWeight;

class Inspector : public Window
{
public:
	Inspector();
	virtual ~Inspector();

	void Draw() override;

	void CheckMeshInfo();

	void ComponentInspector(Component* component);
	void MeshInspector(ComponentMesh* mesh);
	void MatInspector(ComponentMaterial* mat);
	void CamInspector(ComponentCamera* cam);
	void TransformInspector(ComponentTransform* transform); 
	void BillboardInspector(ComponentBillboard* billboard);
	void SkeletonInspector(ComponentSkeleton* skel);
	void AnimationInspector(ComponentAnimation* anim);

public:
	int total_num_vertex;
	int total_num_faces;

	std::string tex_name;

	bool check_info;

	bool radio_world = true;
	bool radio_local = false;

	bool main_cam = false;

	ImVec2 show_size = { 128,128 };

	std::vector<const char*> last_skeleton_bones;
	std::vector<const char*> last_bone_weights;
	ResourceSkeleton* last_skeleton;
	Bone* last_bone;
	BoneWeight* last_weight;

	int bone_ind = 0;
	int weight_ind = 0;
	bool trigger_new_read;
};

#endif
