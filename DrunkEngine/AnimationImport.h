#ifndef _Animation_Import_
#define _Animation_Import_

#include "Assimp/include/scene.h"
#include "GameObject.h"

struct ResourceAnimation;
class MetaAnimation;
class ComponentAnimation;
struct BoneCrumb;

struct AnimToExport
{
	std::vector<aiNodeAnim*> AnimNodes;
	aiAnimation* LinkedAnim;

	~AnimToExport()
	{
		LinkedAnim = nullptr;
		AnimNodes.clear();
	}
};
	
	
	
	

class AnimationImport {
public:
	AnimationImport() { Init(); };
	~AnimationImport() {};

	void Init();

	AnimToExport PrepSkeletonAnimationExport(std::multimap<uint, BoneCrumb*>& Skeleton, aiAnimation* anim);

	void ExportSkelAnimation(std::multimap<uint, BoneCrumb*>& Skeleton, aiAnimation* anim, const char* name);

	void ExportAIAnimation(const aiAnimation* anim, const int& anim_id, const char* name);
	ResourceAnimation* LoadAnimation(const char* file);
	void LinkAnim(DGUID fID, ComponentAnimation* anim);

	void ExportMeta(const aiAnimation* anim, const int& anim_id, std::string path);
	void LoadMeta(const char* file, MetaAnimation* meta);
};
#endif