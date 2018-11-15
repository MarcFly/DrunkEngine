#ifndef _Prefab_Import_
#define _Prefab_Import_

#include "Assimp/include/scene.h"
#include "GameObject.h"

struct ResourcePrefab;
class MetaPrefab;

class PrefabImport {
public:
	PrefabImport() { Init(); };
	~PrefabImport() {};

	void Init();

	void ExportAINode(const aiNode* obj, JSON_Array* go, const Uint32 par_UUID);

	ResourcePrefab* LoadPrefab(const char* file);

	void ExportMeta(const aiNode* obj, std::string& path);
	void LoadMeta(const char* file, MetaPrefab* meta);

	void CopyPrefab(DGUID fID, GameObject* gobj);
	
};

#endif