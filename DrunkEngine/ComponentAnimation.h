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
	void EditorUpdate(const float dt);
	void Draw();
	void CleanUp();

	void LinkMesh();

	void Load(const JSON_Object* comp);
	void Save(JSON_Array* comps);

public:
	ResourceAnimation* r_anim;
	ComponentMesh* c_mesh;
	ComponentSkeleton* c_skel;
	int duration; // In Frames
	int tickrate;
	int anim_blend;

	float timer;
	float curr_time;

	

public:
	void SetBaseVals()
	{
		type = CT_Animation;
		multiple = true;
		parent = nullptr;

		r_anim = nullptr;
		c_mesh = nullptr;
		c_skel = nullptr;
		timer = 0;
		anim_blend = 0;
	}
};

#endif