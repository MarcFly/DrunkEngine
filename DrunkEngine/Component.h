#ifndef _COMPONENT_
#define _COMPONENT_

enum CTypes
{
	CT_Error = -1,
	CT_Mesh,
	CT_Material,
	CT_Camera,
	CT_Transform,

	CT_Max
};

class Component
{
public:
	Component() {};
	~Component() {};

	virtual void Init() {};
	virtual void Start() {};
	virtual void PreUpdate() {};
	virtual void Update() {};
	virtual void Draw() {};
	virtual void PostUpdate() {};
	virtual void CleanUp() {};

	virtual void Load() {};
	virtual void Save() {};

public:
	
	CTypes type;
	bool multiple;

public:
	

};

#endif