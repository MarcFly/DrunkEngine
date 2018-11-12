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
	char HexID[64];

	bool last_comp = true;

	DGUID* me = this;

	DGUID()
	{
		HexID[0] = '\0';
	}
	DGUID(const char* data)
	{
		if (data == nullptr || std::string(data) == "")
			HexID[0] = '\0';
		else
			memcpy(&HexID[0], GetHexID(data).c_str(), 64);		
	}

	const char* operator=(const char* hex)
	{
		memcpy(&HexID[0], hex, 64);
		return HexID;
	}

	bool operator==(const char* cmp_id)
	{
		return (std::string(cmp_id) == std::string(HexID));
	}
	bool operator==(std::string cmp_id)
	{
		return (cmp_id == std::string(HexID));
	}
	bool operator==(DGUID cmp_id)
	{
		return (std::string(cmp_id.HexID) == std::string(HexID));
	}	
	bool operator<(const DGUID cmp_id) const
	{
		/*bool ret = false;
		for (int i = 0; i < 64 && ret == false; i++)
			ret = cmp_id.HexID[i] < HexID[i];
		return !ret;*/
		return cmp_id.CheckSum() < CheckSum();
	}
	bool operator==(const DGUID cmp_id)const
	{
		bool ret = false;
		std::vector<uint> f = TrueComp();
		std::vector<uint> s = cmp_id.TrueComp();
		int victor = 0;
		for (int i = 0; i < 64; i++)
			if (HexID[i] < cmp_id.HexID[i])
			{
				ret = HexID[i] == cmp_id.HexID[i];
				victor++;
			}
		if (ret)
			return !ret;

		if (victor > 32)
			return true;
		else if (victor < 32)
			return false;
		else
		{
			int cs_cmp = (CheckSum() - cmp_id.CheckSum());
			if (cs_cmp > 0)
				return true;
			else
				return false;
		}

	}
	int CheckSum() const {
		int ret = 0;
		for(int i = 0; i < 64; i++)
			ret += HexID[i];
		return ret;
	}
	std::vector<uint> TrueComp() const
	{
		std::vector<uint> ret;
		for (int i = 0; i < 64; i++)
			ret.push_back(HexID[i] + 1000 * HexID[i + 1]);

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