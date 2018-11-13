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

class MeshImport;
class MatImport;

enum FileType
{
	FT_Error = -1,
	FT_Texture,
	FT_Object,
	FT_New_Object,
	FT_Material,
	FT_Mesh,

	FT_Files_Max
};

class ModuleImport : public Module {
public:
	ModuleImport(bool start_enabled = true);
	
	~ModuleImport() {};

	bool Init();

	bool CleanUp();

	void ImportFBX(const char* path);
	
	GameObject* ImportGameObject(const char* path, GameObject* par);
	GameObject* ImportGameObject(const char* path, const aiScene* scene, const aiNode * obj_node, GameObject* par);
	void LoadSceneData(const char* path, const aiScene* scene);

	void ExportScene(const char* scene);

	void LoadFile(char* file);
	FileType CheckExtension(std::string& ext);
	void LoadFileType(char* file, FileType type);
	std::string IsImported(const char* file);

public:
	MeshImport* mesh_i;
	MatImport* mat_i;

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
		/*CreateHiddenDir(".\\Library\\Meshes");
		CreateHiddenDir(".\\Library\\Materials");
		CreateHiddenDir(".\\Library\\Textures");
		CreateHiddenDir(".\\Library\\Cameras");*/
	}


};

#endif