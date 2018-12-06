#ifndef _INSPECTOR_WINDOW_
#define _INSPECTOR_WINDOW_

#include "Window.h"
#include "ModuleScene.h"

class GameObject;
class SkeletonInspectorWin;
struct Bone;
struct BoneWeight;

class Inspector : public Window
{
public:
	Inspector();
	virtual ~Inspector();

	void Draw() override;

	void CheckMeshInfo();

	void ComponentInspector(Component* component, const int& cmpt_id);
	void MeshInspector(ComponentMesh* mesh, const int& cmpt_id);
	void MatInspector(ComponentMaterial* mat, const int& cmpt_id);
	void CamInspector(ComponentCamera* cam, const int& cmpt_id);
	void TransformInspector(ComponentTransform* transform, const int& cmpt_id);
	void BillboardInspector(ComponentBillboard* billboard, const int& cmpt_id);
	
	void AnimationInspector(ComponentAnimation* anim, const int& cmpt_id);

	void SkeletonInspector(ComponentSkeleton* skel, const int& cmpt_id);

public:
	int total_num_vertex;
	int total_num_faces;

	std::string tex_name;

	bool check_info;

	bool radio_world = true;
	bool radio_local = false;

	bool main_cam = false;

	ImVec2 show_size = { 128,128 };

	// Skeleton Inspector
	SkeletonInspectorWin* skel_ins = nullptr;


};

#endif
