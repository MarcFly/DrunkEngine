#ifndef _RESOURCE_
#define _RESOURCE_

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

struct GUID
{
	char HexID[64];

	GUID()
	{
		HexID[0] = '\0';
	}
	GUID(const char* hex)
	{
		memcpy(HexID, hex, 64);
	}

	bool operator==(const char* cmp_id)
	{
		return (std::string(cmp_id) == std::string(HexID));
	}
	bool operator==(std::string& cmp_id)
	{
		return (cmp_id == std::string(HexID));
	}
	bool operator==(const GUID& cmp_id)
	{
		return (std::string(cmp_id.HexID) == std::string(HexID));
	}
};

union Resource
{
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
	MetaResource();
	~MetaResource();

	ResourceTypes type;
	std::string file;
	uint UseCount = 0;
	bool to_pop = false;
	Resource Asset;
};

#endif