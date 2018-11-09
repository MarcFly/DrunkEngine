#ifndef _RESOURCE_MANAGER_
#define _RESOURCE_MANAGER_

#include "Module.h"
#include <string>
#include "FileHelpers.h"
#include <map>

class GameObject;
class Component;
class Texture;

enum ResourceTypes
{
	RT_Error = -1,
	RT_Mesh,
	RT_GameObject,
	RT_Material,
	RT_Texture,

	RT_MAX
};

struct LibraryResource;

union Resource
{
	struct go
	{
		GameObject* ptr = nullptr;
	} gameobject;
	struct cmpnt
	{
		Component* ptr = nullptr;
	} component;
	struct tex
	{
		Texture* ptr = nullptr;
	} texture;

	LibraryResource* par = nullptr;
};

struct LibraryResource
{
	ResourceTypes type;
	std::string file;
	uint UseCount = 0;
	bool to_pop = false;
	Resource Asset;
};

struct UID
{
	char HexID[64];

	UID(const char* hex)
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
};

class ModuleResourceManager : public Module {
public:
	ModuleResourceManager(bool start_enabled = true);

	~ModuleResourceManager() {};

	bool Init();

	bool CleanUp();

public:
	std::map<UID, LibraryResource> Library;

};



#endif