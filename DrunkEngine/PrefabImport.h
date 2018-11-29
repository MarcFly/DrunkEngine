#ifndef _Prefab_Import_
#define _Prefab_Import_

#include "Assimp/include/scene.h"
#include "GameObject.h"

enum NodeToIgnore {
	// Are Ignored
	IN_Error = 000,
	IN_Joint = 001,
	IN_Handle = 002,

	// Are Not Ignored
	IN_NOT_IGNORED,
	// Ignored
	IN_Max
};

struct ResourcePrefab;
class MetaPrefab;

class PrefabImport {
public:
	PrefabImport() { Init(); };
	~PrefabImport() {};

	void Init() {};

	void ExportAINode(const aiScene* scene, const aiNode* obj, JSON_Array* go, const Uint32 par_UUID, const char* name);
	void ExportTransformNode(JSON_Array* comps, const aiMatrix4x4* trans);
	void ExportMeshNode(JSON_Array* comps, const aiMesh* mesh, const int& mesh_id, std::string name);
	void ExportMatNode(JSON_Array* comps, const aiMaterial* mat, const int& mat_id, std::string name);
	void ExportBonesNode(JSON_Array* comps, const aiMesh* mesh, const int& mesh_id, std::string name);
	void ExportAnimNode(JSON_Array* comps, const aiAnimation* anim, const int& anim_id, std::string name);
	ResourcePrefab* LoadPrefab(const char* file);

	// For Prefab Meta, not needed for scene
	void ExportMeta(const aiNode* obj, std::string& path);
	void LoadMeta(const char* file, MetaPrefab* meta);

	void CopyPrefab(DGUID fID, GameObject* gobj);
	
	GameObject* ImportGameObject(const char* path, JSON_Value* go);

	bool IgnoredNodes(const char* node_name);
	bool IgnoredStrings(const char* node_name);
};

#endif