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

class ModuleImport : public Module {
public:
	ModuleImport() {};
	
	~ModuleImport() {};

	bool Init();

	bool CleanUp();

	void ImportFBX(const char* path);
	
	GameObject* ImportGameObject(const char* path);
	GameObject* ImportGameObject(const char* path, const aiScene* scene, const aiNode * obj_node, GameObject* par);

	ComponentMaterial* ImportMaterial(const char* path);
	ComponentMaterial* ImportMaterial(const aiMaterial* mat, GameObject* par);

	Texture* ImportTexture(const char* path, ComponentMaterial* par);

	ComponentMesh* ImportMesh(const char* mesh, GameObject* par);
	//ComponentMesh* ImportMesh(const aiMesh* mesh, GameObject* par);

	void ExportScene(const char* scene);

	void ExportTexture(const char* path);

	void ExportMesh(const aiScene* scene, const int& mesh_id, const char* path = nullptr);
	void ExportIndexNormals(const int& ind, std::vector<GLfloat>& normals, std::vector<GLuint>& index, std::vector<GLfloat>& vertex);
	void ExportBBox(char* data, const int& num_vertex);
	//void ExportNode(const aiScene* scene, const aiNode* node);

	void SerializeSceneData();

	void SerializeObjectData(const GameObject* obj, std::ofstream& file);

	

public:


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
	void TabFile(const int& tabs, std::ofstream& file)
	{
		for (int i = 0; i < tabs; i++)
			file << "\t";
	}

};

#endif