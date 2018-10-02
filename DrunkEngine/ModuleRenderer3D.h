#ifndef _MODULE_RENDERER_3D_H_
#define _MODULE_RENDERER_3D_H_

#include "Module.h"
#include "Globals.h"
#include "Light.h"

#include "glmath/glmath.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);
	void ChangeVsync();
	bool CheckGLError();
	void RenderGrid();

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	bool vsync;
};

#endif