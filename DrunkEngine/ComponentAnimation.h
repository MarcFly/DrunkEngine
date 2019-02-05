#ifndef _COMPONENT_ANIMATION_
#define _COMPONENT_ANIMATION_

#include "Module.h"
#include "Component.h"

struct ResourceAnimation;
struct AnimChannel;
class GameObject;

struct VirtualAnimation
{
	int start = 0;
	int end = 0;
	int tickrate = 1;
	int blend_time = 1;
	int next_animation = 0;
};

enum BlendPhase
{
	Start,
	MidBlend,
	End
};

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

	void BlendFrom(AnimChannel* curr_channel);
	void AnimateSkel(AnimChannel* curr_channel);
public:
	ResourceAnimation* r_anim;
	ComponentMesh* c_mesh;
	ComponentSkeleton* c_skel;

	std::vector<VirtualAnimation> anims;

	int curr_animation;
	int prev_animation;

	float timer;
	float blend_timer;
	int blend_timer_loops;

	BlendPhase phase;

	bool playing;

	int duration;

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
		blend_timer = 0;

		phase = BlendPhase::End;

		curr_animation = 0;
		prev_animation = 0;
		blend_timer_loops = 0;

		playing = true;
	}

};

#endif