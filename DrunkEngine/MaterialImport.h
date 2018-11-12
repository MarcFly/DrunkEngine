#ifndef _MATERIAL_IMPORT_
#define _MATERIAL_IMPORT_

#include "Assimp/include/scene.h"
#include "ComponentMaterial.h"

class GameObject;
struct ResourceMaterial;
class MetaMat;
class MetaTexture;

class MatImport {
public:
	MatImport() { Init(); };
	~MatImport() {};

	void Init();

	void ExportMat(const aiScene * scene, const int& mat_id, const char * path);
	void ExportMat(const ComponentMaterial* mat);
	void ExportTexture(const char* path, const char* full_path = nullptr);
	void ExportTexture(ResourceTexture* tex);

	ResourceMaterial* LoadMat(const char* file);
	void ExportMeta(const aiScene* scene, const int& mat_id, std::string path, char* data);
	void LoadMeta(const char* file, MetaMat* meta);
	void LinkMat(DGUID fID, ComponentMaterial* mat);

	ResourceTexture* LinkTexture(DGUID fID);
	ResourceTexture* LoadTexture(const char* file);
	void LoadMetaTex(const char* file, MetaTexture* meta);
	void ExportMetaTex(std::string path);
};

#endif