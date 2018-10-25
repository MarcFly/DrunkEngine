#ifndef _MODULE_RENDERER_3D_H_
#define _MODULE_RENDERER_3D_H_

#include "Module.h"
#include "Globals.h"
#include "Light.h"

#include "GLEW/include/GL/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "glmath/glmath.h"

#define MAX_LIGHTS 8
#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128

enum TexParams {
	Err = 0,

	TP_TEXTURE_COMPARE_MODE,
	TP_COMPARE_REF,
	TP_NONE,

	TP_TEXTURE_LOD_BIAS,

	TP_TEXTURE_FILTERS, // Filters start here, mag and min types
	TP_NEAREST,
	TP_LINEAR,
	TP_NEAREST_MIPMAP_NEAREST,
	TP_LINEAR_MIPMAP_NEAREST,
	TP_NEAREST_MIPMAP_LINEAR,
	TP_LINEAR_MIPMAP_LINEAR,

	TP_TEXTURE_MIN_LOD,
	TP_TEXTURE_MAX_LOD,
	TP_TEXTURE_MAX_LEVEL,

	// Wraps
	TP_TEXTURE_WRAP, // Wraps start here

	// Wrap Modes
	TP_CLAMP_TO_EDGE,	// coordinades clamped to range based on texture size
	TP_CLAMP_TO_BORDER,	// similar to edge but in borders data is set by border color
	TP_MIRRORED_REPEAT,	// coordinates set to the fractional part if goes beyond 1, so 1.1 = 1/1.1
	TP_REPEAT,			// Integer of coordinates ignored, creatign repetition pattern after 1.0, 1.1 = 0.1
	TP_MIRROR_CLAMP_TO_EDGE,	// repeat for 1 more int (until 2) then clamps to edge


};

struct ComponentMesh;

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void Render(bool use_texture);
	void OnResize();
	void ChangeVsync();
	bool CheckGLError();
	void RenderGrid();
	void RenderBoundBox(ComponentMesh* mesh);
	void SwapWireframe(bool active);

	void SetTextureParams();
	void GenTexParams();

	void InitCheckTex();

public:
	bool Load(JSON_Value* root_value);
	bool Save(JSON_Value* root_value);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	float4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	bool vsync;

	// Options
	bool depth_test;
	bool cull_face;
	bool lighting;
	bool color_material;
	bool texture_2d;
	bool wireframe;
	bool faces;
	bool render_normals;
	float normal_length;

	bool bounding_box;

	// Value as our Enum type
	int curr_tws, curr_twt, curr_tmagf, curr_tminf;
	// Value as GL Type
	uint tws, twt, tmagf, tminf;

  // Checker Texture
	GLubyte checkTexture[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
  
};

#endif