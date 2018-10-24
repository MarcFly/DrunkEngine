#ifndef _COMPONENT_
#define _COMPONENT_

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

};

#endif