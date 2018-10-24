#ifndef _MATERIAL_IMPORT_
#define _MATERIAL_IMPORT_

#include "Assimp/include/scene.h"
#include "ComponentMaterial.h"

class GameObject;

class MatImport {
public:
	MatImport() { Init(); };
	~MatImport() {};

	void Init();

	ComponentMaterial* ImportMat(const char* mat, GameObject* par);
	Texture* ImportTexture(const char* path, ComponentMaterial* par);

	void ExportMat(const aiScene * scene, const int& mat_id, const char * path);
	void ExportTexture(const char* path, const char* full_path = nullptr);
};

#endif
/*
if (!check) // Check from imported textures folder
{
	std::string new_file_path = path;
	new_file_path = new_file_path.substr(new_file_path.find_last_of("\\/") + 1);

	new_file_path = "./Library/Textures/" + new_file_path; // Have to set new directories

	check = ilLoadImage(new_file_path.c_str());
	if (check)
		App->ui->console_win->AddLog("Texture found in Imported Directories");
}*/