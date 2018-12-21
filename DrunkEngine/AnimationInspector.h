#ifndef _ANIMATION_INSPECTOR_
#define _ANIMATION_INSPECTOR_

#include "Window.h"

class ComponentAnimation;

class AnimationInspectorWin : public Window
{
public:
	AnimationInspectorWin();
	virtual ~AnimationInspectorWin();

	void Draw() override;

public:
	ComponentAnimation* anim;

	AnimChannel* sel_bone_anim = nullptr;

	//bool interpolation = false;

	ImVec2 barMovement = { 0, 0 };
	ImVec2 mouseMovement = { 0, 0 };
	bool dragging = false;
	bool scrolled = false;

	float recSize = 700;
	int zoom = 50;
	float speed = 0.0f;
	float progress = 0.0f;
	float winSize = 0.0f;

};

#endif