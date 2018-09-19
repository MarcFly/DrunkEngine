#ifndef _MODULE_RENDERER_3D_H_
#define _MODULE_RENDERER_3D_H_

#include "Module.h"
#include "Globals.h"
#include "MGL\MathGeoLib.h"
#include "Light.h"
#include "Bullet\include\LinearMath\btMatrixX.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	btMatrix3x3 NormalMatrix;
	//float4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	btMatrixX<float> ModelMatrix = btMatrixX<float>(4,4), ViewMatrix = btMatrixX<float>(4, 4), ProjectionMatrix = btMatrixX<float>(4, 4);
};

#endif