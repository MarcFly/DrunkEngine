#ifndef _MODULE_IMPORT_
#define _MODULE_IMPORT_

#include "Module.h"
#include "GameObject.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "GLEW/include/GL/glew.h"
#include "Assimp/include/scene.h"
#include "Color.h"
#include "Timer.h"

#include <iostream>
#include <fstream>

#include "FileHelpers.h"
#include "ResourceAnimation.h"

class MeshImport;
class MatImport;
class PrefabImport;
class SkeletonImport;
class AnimationImport;

class ModuleImport : public Module {
public:
	ModuleImport(bool start_enabled = true);
	
	~ModuleImport() {};

	bool Init();

	bool CleanUp();
	
	//GameObject* ImportGameObject(const char* path, const aiScene* scene, const aiNode * obj_node, GameObject* par);
	void LoadScene(const char* path);

	void ExportScene(const char* path);
	void ExportSceneNodes(const char* path, std::vector<const aiNode*>& NodesWithSkeleton, std::vector<std::vector<AnimToExport>>& Animations, const aiNode* root_node, const aiScene* aiscene);

	void LoadFile(char* file);
	
	void LoadFileType(char* file, FileType type);

public:
	MeshImport* mesh_i;
	MatImport* mat_i;
	PrefabImport* prefab_i;
	SkeletonImport* skel_i;
	AnimationImport* anim_i;

	std::string tex_folder;
	std::string mesh_folder;
	std::string mat_folder;
	std::string cam_folder;
	std::string obj_folder;

	Timer Imp_Timer;

public:
	
	int ParCount(GameObject* obj)
	{
		int ret = 0;

		while (obj != nullptr)
		{
			ret++;
			obj = obj->parent;
		}

		return ret;
	}
	
	void SetDirectories()
	{
		CreateHiddenDir(".\\Library");
	}


};

#endif