#include "Globals.h"
#include "Light.h"
#include <gl/GL.h>
//#include <gl/GLU.h>

Light::Light() : ref(-1), on(false), position(0.0f, 0.0f, 0.0f)
{}

void Light::Init()
{
	glLightfv(ref, GL_AMBIENT, &ambient);
	glLightfv(ref, GL_DIFFUSE, &diffuse);
}

void Light::SetPos(float x, float y, float z)
{
	position.setX(x);
	position.setY(y);
	position.setZ(z);
}

void Light::Render()
{
	if(on)
	{
		float pos[] = {position.getX(), position.getY(), position.getZ(), 1.0f};
		glLightfv(ref, GL_POSITION, pos);
	}
}

void Light::Active(bool active)
{
	
	if(on != active)
	{
		on = !on;

		if(on)
			glEnable(ref);
		else
			glDisable(ref);
	}
	
}