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

	void ExportAIMat(const aiMaterial * mat, const int& mat_id, const char * path);
	void ExportMat(const ComponentMaterial* mat);
	void ExportILTexture(const char* path, const char* full_path = nullptr);
	void ExportTexture(ResourceTexture* tex);

	ResourceMaterial* LoadMat(const char* file);
	void ExportMeta(const aiMaterial* mat, const int& mat_id, std::string& path);
	void LoadMeta(const char* file, MetaMat* meta);
	void LinkMat(DGUID fID, ComponentMaterial* mat);

	ResourceTexture* LinkTexture(DGUID fID);
	ResourceTexture* LoadTexture(const char* file);
	void LoadMetaTex(const char* file, MetaTexture* meta);
	void ExportMetaTex(std::string path);
};

#endif