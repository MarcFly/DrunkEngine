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
	GameObject* ImportGameObject(const aiScene* scene, const aiNode * obj_node, GameObject* par);

	ComponentMaterial* ImportMaterial(const char* path);
	ComponentMaterial* ImportMaterial(const aiMaterial* mat, GameObject* par);

	Texture* ImportTexture(const char* path, ComponentMaterial* par);

	ComponentMesh* ImportMesh(const char* mesh);
	ComponentMesh* ImportMesh(const aiMesh* mesh, GameObject* par);

	void ExportScene(const char* scene);

	void ExportTexture(const char* path);

	void ExportMesh(const aiMesh* mesh);
	void ExportMeshNormals(char* data, const int& index, const unsigned int& vertex_size, const unsigned int& index_size);
	void ExportBBox(char* data, const int& num_vertex, const unsigned int& start_size);
	//void ExportNode(const aiScene* scene, const aiNode* node);

	void SerializeSceneData();

	void SerializeObjectData(const GameObject* obj, std::ofstream& file);

	

public:


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
	void TabFile(const int& tabs, std::ofstream& file)
	{
		for (int i = 0; i < tabs; i++)
			file << "\t";
	}

};

#endif