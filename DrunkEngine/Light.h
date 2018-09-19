#ifndef _LIGHT_H_
#define _LIGHT_H_


#pragma once
#include "Color.h"
#include "MGL\MathGeoLib.h"

struct Light
{
	Light();

	void Init();
	void SetPos(float x, float y, float z);
	void Active(bool active);
	void Render();

	Color ambient;
	Color diffuse;
	vec position;

	int ref;
	bool on;
};

#endif // !_LIGHT_H_