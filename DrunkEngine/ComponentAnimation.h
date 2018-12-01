#ifndef _COMPONENT_ANIMATION_
#define _COMPONENT_ANIMATION_

#include "Module.h"
#include "Component.h"

struct ResourceAnimation;

class GameObject;

class ComponentAnimation : public Component
{
public:
	ComponentAnimation();
	ComponentAnimation(GameObject* par);
	~ComponentAnimation();

	void Start();
	void Update(const float dt);
	void Draw();
	void CleanUp();

	void LinkMesh();

	void Load(const JSON_Object* comp);
	void Save(JSON_Array* comps);

public:
	ResourceAnimation* r_anim;
	ComponentMesh* c_mesh;
	int duration;
	int tickrate;

public:
	void SetBaseVals()
	{
		type = CT_Animation;
		multiple = true;
		parent = nullptr;

		r_anim = nullptr;
		c_mesh = nullptr;
	}
};

#endif