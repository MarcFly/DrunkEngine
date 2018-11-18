#ifndef _RESOURCE_MANAGER_
#define _RESOURCE_MANAGER_

#include "Module.h"
#include <string>
#include "FileHelpers.h"
#include <map>
#include "Resource.h"

class ModuleResourceManager : public Module {
public:
	ModuleResourceManager(bool start_enabled = true);

	~ModuleResourceManager() {};

	bool Init();

	bool PostUpdate(float dt);

	bool CleanUp();
	
	DGUID AddResource(const char* file);
	MetaResource* NewResource(FileType type);

public:
	std::map<DGUID, MetaResource*, std::equal_to<DGUID>> Library;
	
	bool InLibrary(DGUID& check)
	{
		if (!check.CheckValidity())
			return false;
		else
		{
			std::map<DGUID, MetaResource*>::iterator it = Library.find(check);
			return (it != Library.end());
		}
	}

	void Unused(DGUID uid)
	{
		Library.at(uid)->UseCount--;
	}

};



#endif