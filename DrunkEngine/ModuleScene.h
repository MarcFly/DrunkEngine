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
	void DestroyObject(const int& index);
	void DestroyTexture(GameObject* curr_obj, const int& tex_ind);

	void GenTexParams();
	
	void SetCurrTexTo(GameObject& curr_obj, const int tex_ind);

	void SetMeshBoundBox(ComponentMesh &mesh);
	float SetObjBoundBox(GameObject &object, const aiScene* scene);

	void DrawMesh(const ComponentMesh* mesh, bool use_texture);

	bool Load(JSON_Value* root_value);
	bool Save(JSON_Value* root_value);

public:
	GameObject* Root_Object;

	std::string scene_folder;
	std::string tex_folder;

public:
	GameObject* getRootObj() const { return Root_Object; }

	vec3 getObjectCenter(const GameObject* obj);

	int GetDevILVer();

	int GetAssimpMajorVer() { return aiGetVersionMajor(); };
	int GetAssimpMinorVer() { return aiGetVersionMinor(); };
	int GetAssimpVersionRevision() { return aiGetVersionRevision(); };

	void SetParents() 
	{
		for (int j = 0; j < Objects.size(); j++)
			for (int k = 0; k < Objects[j].meshes.size(); k++)
				Objects[j].meshes[k].parent = &Objects[j];
	}
};

#endif