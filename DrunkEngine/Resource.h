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
	bool operator<(DGUID const& cmp_id) const
	{
		bool ret = !(std::string(cmp_id.HexID) == std::string(HexID));
		return ret;
	}
};

union Resource
{
	Resource() {};
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
	MetaResource() {};
	virtual ~MetaResource() {};

	ResourceTypes type;
	std::string file;
	uint UseCount = 0;
	bool to_pop = false;
	Resource Asset;

	virtual void LoadMetaFile(const char* path) {};
};

#endif