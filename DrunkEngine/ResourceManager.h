#ifndef _RESOURCE_MANAGER_
#define _RESOURCE_MANAGER_

#include "Module.h"
#include <string>
#include "FileHelpers.h"

class Resource;

enum ResourceTypes
{
	RT_Error = -1,

	RT_MAX
};

struct LibraryResource
{
	ResourceTypes type;
	std::string file;
	uint UseCount = 0;
	char HexID[64];
	bool to_pop = false;
	Resource* Asset = nullptr;
	
};

class ResourceManager : public Module {
public:
	ResourceManager(bool start_enabled = true);

	~ResourceManager() {};

	bool Init();

	bool CleanUp();

public:
	std::vector<LibraryResource> Library;

};

#endif