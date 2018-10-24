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

	ComponentMaterial* ImportMaterial(const aiMaterial* mat, GameObject* par);
	Texture* ImportTexture(const char* path, ComponentMaterial* par);

	void ExportTexture(const char* path);
};

#endif