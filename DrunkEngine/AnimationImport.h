#ifndef _Animation_Import_
#define _Animation_Import_

#include "Assimp/include/scene.h"
#include "GameObject.h"

struct ResourceAnimation;
class MetaAnimation;
class ComponentAnimation;

class AnimationImport {
public:
	AnimationImport() { Init(); };
	~AnimationImport() {};

	void Init() {};

	void ExportAIAnimation(const aiAnimation* anim, const int& anim_id, const char* name);
	void LoadAnimation(const char* file);
	void LinkAnim(DGUID fID, ComponentAnimation* anim);

	void ExportMeta(const aiAnimation* anim, std::string& path);
	void LoadMeta(const char* file, MetaAnimation* meta);
};
#endif