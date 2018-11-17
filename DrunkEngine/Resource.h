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
		MD5ID[0] = '\0';
	}
	DGUID(const char* data)
	{
		if (data == nullptr || std::string(data) == "")
			MD5ID[0] = '\0';
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
	bool operator==(const DGUID cmp_id)const
	{
		bool ret = false;
		for (int i = 0; i < 32; i++)
		{
			ret = MD5ID[i] == cmp_id.MD5ID[i];
			if (!ret)
				break;
		}
		if (ret)
			return !ret;
		else
		{
			return TrueComp(cmp_id);
		}

	}

	bool TrueComp(const DGUID cmp_id) const;
	std::vector<uint> TrueSum() const;
	bool operator==(DGUID cmp_id);
};

union Resource
{
	Resource() {};
	Resource(MetaResource* parent) : par{parent} {};
	
	// Dirty check ptr to know if it is in mem, ask check as a global mem check
	struct check
	{
		bool* ptr = nullptr;
	}inmem;

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
	bool IsLoaded() {return inmem.ptr != nullptr;};
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