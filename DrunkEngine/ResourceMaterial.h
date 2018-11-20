#ifndef _Res_Mat_
#define _Res_Mat_

#include "Color.h"
#include "Resource.h"
#include "MaterialImport.h"
#include "Application.h"

struct ResourceTexture;

struct ResourceMaterial
{
	uint NumDiffTextures = 0;
	std::vector<DGUID> textures;
	uint NumProperties = 0;
	
	~ResourceMaterial();
	void UnloadMem();

	// Material Properties
	Color default_print = { 1,1,1,1 };	
};

class MetaMat : public MetaResource
{
public:
	MetaMat() { type = RT_Material; };
	~MetaMat() {};

	void LoadMetaFile(const char* file)
	{
		App->importer->mat_i->LoadMeta(file, this);
	}
};

#endif