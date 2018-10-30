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

	ComponentMaterial* ImportMat(const char* file, ComponentMaterial* mat, const char* Dir = nullptr);
	Texture* ImportTexture(const char* path, ComponentMaterial* par, const char* Dir = nullptr);

	void ExportMat(const aiScene * scene, const int& mat_id, const char * path);
	void ExportMat(const ComponentMaterial* mat);
	void ExportTexture(const char* path, const char* full_path = nullptr);
	void ExportTexture(Texture* tex);
};

#endif