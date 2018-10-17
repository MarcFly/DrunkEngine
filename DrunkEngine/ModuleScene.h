#ifndef _LOAD_MESH_H_
#define _LOAD_MESH_H_

#include "Module.h"
#include "GameObject.h"
#include "Assimp/include/version.h"
#include "Assimp/include/scene.h"

class vec3;

class ModuleScene : public Module
{
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene() {};

	bool Init();
	bool Start();

	bool CleanUp();

	bool LoadFromFile(const char* file_path);

	//bool CreatePrimitiveObject(const vec& center, PCube& sphere);
	//bool CreatePrimitiveObject(const vec& center, PSphere& sphere);
	//bool CreatePrimitiveObject(const vec& center, PSphere& sphere);

	bool LoadTextCurrentObj(const char* path, GameObject* curr_obj);
	bool DestroyScene();
	
	void SetCurrTexTo(GameObject& curr_obj, const int tex_ind);

	GameObject* GetSelected(GameObject * obj);
	void SetSelectedFalse(GameObject* obj);

	bool Load(JSON_Value* root_value);
	bool Save(JSON_Value* root_value);

public:
	GameObject* Root_Object = nullptr;

	std::string scene_folder;
	std::string tex_folder;

public:
	GameObject* getRootObj() const { return Root_Object; };

	int GetDevILVer();

	int GetAssimpMajorVer() { return aiGetVersionMajor(); };
	int GetAssimpMinorVer() { return aiGetVersionMinor(); };
	int GetAssimpVersionRevision() { return aiGetVersionRevision(); };

	void Draw() { Root_Object->Draw(); };
};

#endif