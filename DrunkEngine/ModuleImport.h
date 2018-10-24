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

#include <iostream>
#include <fstream>

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
	ModuleImport() {};
	
	~ModuleImport() {};

	bool Init();

	bool CleanUp();

	void ImportFBX(const char* path);
	
	GameObject* ImportGameObject(const char* path);
	GameObject* ImportGameObject(const char* path, const aiScene* scene, const aiNode * obj_node, GameObject* par);

	void ExportScene(const char* scene);

	void LoadFile(char* file);
	FileType CheckExtension(std::string& ext);
	void LoadFileType(char* file, FileType type);
public:
	MeshImport* mesh_i;
	MatImport* mat_i;


public:
	
	std::string GetFileName(const char* file)
	{
		std::string ret = file;

		ret = ret.substr(ret.find_last_of("\\/") + 1);

		std::string aux = strrchr(ret.c_str(), '.');
		ret.erase(ret.length() - aux.length()).c_str();

		return ret;
	}

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

	uint GetExtSize(const char* file)
	{
		std::string ret = strrchr(file, '.');

		return ret.length();
	}

};

#endif