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
	RT_GameObject,
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

	const char* operator=(const char* hex)
	{
		memcpy(&MD5ID[0], hex, 64);
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
	bool operator==(DGUID cmp_id)
	{
		return (std::string(cmp_id.MD5ID) == std::string(MD5ID));
	}	
	bool operator<(const DGUID cmp_id) const
	{
		return cmp_id.CheckSum() < CheckSum();
	}
	bool operator==(const DGUID cmp_id)const
	{
		bool ret = false;
		std::vector<uint> f = TrueComp();
		std::vector<uint> s = cmp_id.TrueComp();
		int victor = 0;
		for (int i = 0; i < 32; i++)
		{
			ret = MD5ID[i] == cmp_id.MD5ID[i];
			if (MD5ID[i] < cmp_id.MD5ID[i])
				victor++;
		}
		if (ret)
			return !ret;

		if (victor > 16)
			return false;
		else if (victor < 16)
			return true;
		else
		{
			int cs_cmp = (CheckSum() - cmp_id.CheckSum());
			if (cs_cmp > 0)
				return false;
			else
				return true;
		}

	}
	int CheckSum() const {
		int ret = 0;
		for(int i = 0; i < 32; i++)
			ret += MD5ID[i];
		return ret;
	}
	std::vector<uint> TrueComp() const
	{
		std::vector<uint> ret;
		for (int i = 0; i < 32; i++)
			ret.push_back(MD5ID[i] + 1000 * MD5ID[i + 1]);

		return ret;
	}
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
	bool IsLoaded();
};

class MetaResource
{
public:
	MetaResource() { Asset.par = this; };
	virtual ~MetaResource() {};

	ResourceTypes type;
	std::string file;
	uint UseCount = 0;
	bool to_pop = false;
	Resource Asset;

	virtual void LoadMetaFile(const char* path) {};
};

#endif