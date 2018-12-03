#ifndef _SKELETON_IMPORT_
#define _SKELETON_IMPORT_

#include "Assimp/include/scene.h"
#include "GLEW/include/GL/glew.h"
#include "Globals.h"

#include <vector>
#include <map>

class ComponentSkeleton;
class GameObject;
struct ResourceSkeleton;
struct DGUID;
class MetaSkeleton;

struct BoneCrumb {
	const aiBone* Bone;
	const aiNode* BoneNode;
	const aiMesh* Mesh;	

	uint fast_id;
	uint fast_par_id;
};

class SkeletonImport {
public:
	SkeletonImport() {};
	~SkeletonImport() {};

	void Init();

	void LinkSkeleton(DGUID fID, ComponentSkeleton* skel);
	ResourceSkeleton* LoadSkeleton(const char* file);

	void FindBoneNodes(aiMesh** meshes, const aiNode* node, std::vector<const aiNode*>& bone_nodes);
	void GetSkeletonRoots(std::vector<const aiNode*>& BoneNodes, std::vector<const aiNode*>& SkeletonRoots);
	std::vector<std::multimap<uint, BoneCrumb*>> ReconstructSkeletons(const aiScene* scene, std::vector<const aiNode*>& SkeletonRoots, std::vector<const aiNode*>& BoneNodes);
	
	void ExportMapSkeleton(const aiScene* scene, const aiNode* SkelRoot, std::multimap<uint, BoneCrumb*>& Skeleton, const int& skel_id, const aiNode* root, const char* path);

	void ExportMeta(std::string& filename);
	void LoadMeta(const char* file, MetaSkeleton* meta);

};

#endif