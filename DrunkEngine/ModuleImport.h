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

	void SerializeSceneData();

public:

};

#endif