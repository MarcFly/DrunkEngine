#ifndef _RESOURCE_MANAGER_
#define _RESOURCE_MANAGER_

#include "Module.h"
#include <string>
#include "FileHelpers.h"
#include <map>
#include "Resource.h"

class MetaResource;



class ModuleResourceManager : public Module {
public:
	ModuleResourceManager(bool start_enabled = true);

	~ModuleResourceManager() {};

	bool Init();

	bool CleanUp();
	

public:
	std::map<GUID, MetaResource> Library;
	std::string non_id = "12AE32CB1EC02D01EDA3581B127C1FEE3B0DC53572ED6BAF239721A03D82E126";
public:
	bool InLibrary(GUID& check)
	{
		if (check == non_id)
			return false;
		else
			return (Library.find(check) != Library.end);
	}

};



#endif