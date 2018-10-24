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
	
	GameObject* ImportGameObject(const char* path, GameObject* par);
	GameObject* ImportGameObject(const char* path, const aiScene* scene, const aiNode * obj_node, GameObject* par);

	void ExportScene(const char* scene);

	void LoadFile(char* file);
	FileType CheckExtension(std::string& ext);
	void LoadFileType(char* file, FileType type);
public:
	MeshImport* mesh_i;
	MatImport* mat_i;

	std::string tex_folder;
	std::string mesh_folder;
	std::string mat_folder;
	std::string cam_folder;
	std::string obj_folder;


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

	void SetDirectories()
	{
		CreateDirectory("./Library", NULL);
		SetFileAttributes("./Library", FILE_ATTRIBUTE_HIDDEN);

		CreateDirectory("./Library\\Meshes", NULL);
		SetFileAttributes("./Library\\Meshes", FILE_ATTRIBUTE_HIDDEN);

		CreateDirectory("./Library\\Materials", NULL);
		SetFileAttributes("./Library\\Materials", FILE_ATTRIBUTE_HIDDEN);

		CreateDirectory("./Library\\Textures", NULL);
		SetFileAttributes("./Library\\Textures", FILE_ATTRIBUTE_HIDDEN);

		CreateDirectory("./Library\\Cameras", NULL);
		SetFileAttributes("./Library\\Cameras", FILE_ATTRIBUTE_HIDDEN);
	}

	std::string GetDir(const char* full_path)
	{
		std::string aux = strrchr(full_path, '\\/');
		std::string path = full_path;
		//original_load.substr(aux.length());
		path.erase(path.length() - aux.length() + 1); // + 1 because we can find / but it will ask to erase it for length, +1 will not erase /
		
		return path;
	}

};

#endif