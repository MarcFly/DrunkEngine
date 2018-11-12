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
		for (int i = 0; i < 64 && ret == false; i++)
			ret = cmp_id.HexID[i] < HexID[i];
		return ret;
	}
	int CheckSum() const {
		int ret = 0;
		for(int i = 0; i < 64; i++)
			ret += HexID[i];
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