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
};

#endif