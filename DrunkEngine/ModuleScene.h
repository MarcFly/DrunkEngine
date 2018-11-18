#ifndef _LOAD_MESH_H_
#define _LOAD_MESH_H_

#include "Module.h"
#include "GameObject.h"
#include "Assimp/include/version.h"
#include "Assimp/include/scene.h"

class vec3;
class ComponentCamera;

class ModuleScene : public Module
{
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene() {};

	bool Init();
	bool Start();

	bool CleanUp();

	bool LoadFBX(const char* file_path);
	bool LoadSceneFile(const char* file_path);

	bool LoadTextCurrentObj(const char* path, GameObject* curr_obj);
	bool DestroyScene();
	
	void SetCurrTexTo(GameObject& curr_obj, const int tex_ind);

	bool Load(JSON_Value* root_value);
	bool Save(JSON_Value* root_value);

	std::string SaveScene(const char* filename = "");

	void OrderScene();

	void RecieveEvent(const Event & event);

public:
	std::string scene_folder;

	int GetDevILVer();

	int GetAssimpMajorVer() { return aiGetVersionMajor(); };
	int GetAssimpMinorVer() { return aiGetVersionMinor(); };
	int GetAssimpVersionRevision() { return aiGetVersionRevision(); };

private:
	void SetDefault()
	{
		scene_folder = "Assets\/";
	}
};

#endif