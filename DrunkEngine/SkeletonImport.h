#ifndef _SKELETON_IMPORT_
#define _SKELETON_IMPORT_

#include "Assimp/include/scene.h"
#include "GLEW/include/GL/glew.h"
#include <vector>

class ComponentSkeleton;
class GameObject;
struct ResourceSkeleton;
struct DGUID;
class MetaSkeleton;

class SkeletonImport {
public:
	SkeletonImport() {};
	~SkeletonImport() {};

	void Init();

	void LinkSkeleton(DGUID fID, ComponentSkeleton* skel);
	ResourceSkeleton* LoadSkeleton(const char* file);

	std::vector<const aiNode*> FindSkeletons(const aiScene* scene, const aiNode* root);
	void FindBoneNodes(const aiMesh** scene, const aiNode* node, std::vector<const aiNode*>& bone_nodes);
	void GetSkeletonRoots(std::vector<const aiNode*>& BoneNodes, std::vector<const aiNode*>& SkeletonRoots);
	void ExportAISkeleton(const aiNode* root, const aiMesh* mesh, const int& mesh_id, const char* path = nullptr);

	void ExportMeta(const aiMesh* mesh, const int& mesh_id, std::string path);
	void LoadMeta(const char* file, MetaSkeleton* meta);

};

#endif