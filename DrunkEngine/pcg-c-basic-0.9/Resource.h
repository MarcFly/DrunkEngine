#ifndef _RESOURCE_
#define _RESOURCE_

#include <string>
#include "Globals.h"
#include "FileHelpers.h"

class GameObject;
class ResourceMesh;
struct ResourceMaterial;
struct ResourceTexture;
struct MetaResource;

enum ResourceTypes
{
	RT_Error = -1,
	RT_Prefab,
	RT_Mesh,
	RT_Material,
	RT_Texture,

	RT_MAX
};

struct DGUID
{
	char MD5ID[32];

	DGUID()
	{
		SetInvalid();
	}
	DGUID(const char* data)
	{
		if (data == nullptr || std::string(data) == "")
			SetInvalid();
		else
		{
			std::string ret = GetMD5ID(data);
			memcpy(&MD5ID[0], ret.c_str(), 32);
		}
	}

	DGUID operator=(DGUID cmp_id)
	{
		memcpy(&MD5ID[0], &cmp_id.MD5ID[0], 32);
		return *this;
	}

	const char* operator=(const char* MD5id)
	{
		memcpy(&MD5ID[0], MD5id, 32);
		return MD5ID;
	}

	bool operator==(const char* cmp_id)
	{
		return (std::string(cmp_id) == std::string(MD5ID));
	}
	bool operator==(std::string cmp_id)
	{
		return (cmp_id == std::string(MD5ID));
	}
	bool operator<(const DGUID cmp_id) const
	{
		return cmp_id.TrueSum()[0] < TrueSum()[0];
	}

	bool operator==(DGUID cmp_id);
	bool operator==(const DGUID cmp_id)const;
	
	bool TrueComp(const DGUID cmp_id) const;
	std::vector<uint> TrueSum() const;

	void SetInvalid();
	bool CheckValidity();
	
};

union Resource
{
	Resource() {};
	Resource(MetaResource* parent) : par{parent} {};

	struct mesh
	{
		ResourceMesh* ptr = nullptr;
	} mesh;
	struct material
	{
		ResourceMaterial* ptr = nullptr;
	} mat;
	struct tex
	{
		ResourceTexture* ptr = nullptr;
	} texture;

	MetaResource* par = nullptr;

public:
	void LoadToMem();
	void UnloadFromMem();
	bool IsInUse();
	bool IsLoaded();
};

class MetaResource
{
public:
	MetaResource() {
		Asset.par = this; 
	};
	virtual ~MetaResource() {};

	ResourceTypes type = RT_Error;
	std::string file;
	uint UseCount = 0;
	bool to_pop = false;
	Resource Asset;

	virtual void LoadMetaFile(const char* path) {};
};

#endif