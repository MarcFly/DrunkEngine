#include "Application.h"
#include "ModuleRenderer3D.h"
#include "PhysBody3D.h"
#include "ModuleManageMesh.h"

#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#define GRID_SIZE 10

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	InitCheckTex();
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	bool ret = true;
	vsync = true;
	wireframe = false;
	faces = true;
	render_normals = false;
	normal_length = 1.0f;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	//Create context
	PLOG("Creating 3D Renderer context");
	context = SDL_GL_CreateContext(App->window->window);
	if (GLEW_OK != glewInit())
	{
		PLOG("Failed GLEW Initiation!\n")
	}

	if(context == NULL)
	{
		PLOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	Load(nullptr);

	if(ret == true)
	{
		//Use Vsync
		if(SDL_GL_SetSwapInterval(vsync) < 0)
			PLOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		ret = CheckGLError();

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error;
		ret = CheckGLError();
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

		//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glShadeModel(GL_SMOOTH);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 1.0f, 0.f, 0.5f); // In theory, bright glow green
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // This blend is for transparency, with primitives sorted from far to near, also to antialiased points
		// There are different ways to optimize different effects, polygon optimization is SRC_ALPHA_SATURATE, GL_ONE for example, and disable PolygonSmooth
		glDepthFunc(GL_LEQUAL);

		//Check for error
		ret = CheckGLError();
		

		glEnable(GL_BLEND);
		glEnable(GL_ALPHA_TEST);
    
		if (depth_test)
			glEnable(GL_DEPTH_TEST); // Tests depth when rendering
		if (cull_face)
			glEnable(GL_CULL_FACE); // If you want to see objects interior, turn off
		if (lighting)
			glEnable(GL_LIGHTING); // Computes vertex color from lighting paramenters, else associates every vertex to current color
		if (color_material)
			glEnable(GL_COLOR_MATERIAL); // The color is tracked through ambient and diffuse parameters, instead of static
		if(texture_2d)
			glEnable(GL_TEXTURE_2D); // Texturing is performed in 2D, important for activetexture

		// Something about lights
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		lights[0].Active(true);

		// Something about materials
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);
	
	
	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	Color c = App->camera->background;
	glClearColor(c.r,c.g,c.b,c.a);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	int width, height;
	SDL_GetWindowSize(App->window->window, &width, &height);
	glViewport(0, 0, width, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	RenderGrid();

	// Something Something lights
	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// Do the render of Objects
update_status ModuleRenderer3D::Update(float dt)
{

	if (faces)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		Render(true);
	}

	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0, 0, 0);
		Render(false);
	}

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	PLOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::Render(bool use_texture)
{
	// Render From primitive list
	std::list<PhysBody3D*>::iterator item_render = App->physics->bodies.begin();
	while (item_render != App->physics->bodies.end() && App->physics->bodies.size() > 0) {
		item_render._Ptr->_Myval->mbody->InnerRender();
		item_render++;
	}

	for (int i = 0; i < App->mesh_loader->Meshes.size(); i++)
	{

		App->mesh_loader->DrawMesh(&App->mesh_loader->Meshes[i], use_texture);	

		// Draw elements
		mesh_data* mesh = &App->mesh_loader->Meshes[i];
		{

			// Draw Normals
			if (render_normals)
			{
				glBegin(GL_LINES);
				glColor3f(0.0f, 1.0f, 0.0f);

				for (int i = 0; i < mesh->num_normal / 6; i++)
				{
					glVertex3f(mesh->normal[i * 6], mesh->normal[i * 6 + 1], mesh->normal[i * 6 + 2]);

					vec norm(mesh->normal[i * 6 + 3] - mesh->normal[i * 6], mesh->normal[i * 6 + 4] - mesh->normal[i * 6 + 1], mesh->normal[i * 6 + 5] - mesh->normal[i * 6 + 2]);
					norm = norm.Mul(normal_length);

					glVertex3f(mesh->normal[i * 6] + norm.x, mesh->normal[i * 6 + 1] + norm.y, mesh->normal[i * 6 + 2] + norm.z);
				}

				glEnd();
			}

			glDisableClientState(GL_VERTEX_ARRAY);
		}
	}
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);
	float4x4 temp;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = ProjectionMatrix.perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::ChangeVsync()
{
	
	if (SDL_GL_SetSwapInterval(vsync) < 0)
		PLOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

}

bool ModuleRenderer3D::CheckGLError()
{
	bool ret = true;

	//Check for error
	if (glGetError() != GL_NO_ERROR)
	{
		PLOG("Error initializing OpenGL! %s\n", gluErrorString(glGetError()));
		ret = false;
	}

	return ret;
}

void ModuleRenderer3D::RenderGrid()
{
	for (int i = 0; i < GRID_SIZE * 2 + 1; i++)
	{
		glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);

		//Z
		glVertex3i(GRID_SIZE - i, 0, GRID_SIZE);
		glVertex3i(GRID_SIZE - i, 0, -GRID_SIZE);

		//X
		glVertex3i(-GRID_SIZE, 0, -GRID_SIZE + i);
		glVertex3i(GRID_SIZE, 0, -GRID_SIZE + i);
		glEnd();
	}
}

void ModuleRenderer3D::SwapWireframe(bool active)
{
	if (active)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ModuleRenderer3D::InitCheckTex()
{
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkTexture[i][j][0] = (GLubyte)c;
			checkTexture[i][j][1] = (GLubyte)c;
			checkTexture[i][j][2] = (GLubyte)c;
			checkTexture[i][j][3] = (GLubyte)255;
		}
	}
}

bool ModuleRenderer3D::Load(JSON_Value * root_value)
{
	bool ret = false;

	root_value = json_parse_file("config_data.json");

	depth_test = json_object_dotget_boolean(json_object(root_value), "opengl.depth_test");
	cull_face = json_object_dotget_boolean(json_object(root_value), "opengl.cull_face");
	lighting = json_object_dotget_boolean(json_object(root_value), "opengl.lighting");
	color_material = json_object_dotget_boolean(json_object(root_value), "opengl.color_materials");
	texture_2d = json_object_dotget_boolean(json_object(root_value), "opengl.texture2d");
	wireframe = json_object_dotget_boolean(json_object(root_value), "opengl.wireframe");
	faces = json_object_dotget_boolean(json_object(root_value), "opengl.faces&wireframe");
	render_normals = json_object_dotget_boolean(json_object(root_value), "opengl.normals");
	normal_length = json_object_dotget_number(json_object(root_value), "opengl.normal_length");

	ret = true;
	return ret;
}

bool ModuleRenderer3D::Save(JSON_Value * root_value)
{
	bool ret = false;


	root_value = json_parse_file("config_data.json");
	JSON_Object* root_obj = json_value_get_object(root_value);

	json_object_dotset_boolean(root_obj, "opengl.depth_test", depth_test);
	json_object_dotset_boolean(root_obj, "opengl.cull_face", cull_face);
	json_object_dotset_boolean(root_obj, "opengl.lighting", lighting);
	json_object_dotset_boolean(root_obj, "opengl.color_materials", color_material);
	json_object_dotset_boolean(root_obj, "opengl.texture2d", texture_2d);
	json_object_dotset_boolean(root_obj, "opengl.wireframe", wireframe);
	json_object_dotset_boolean(root_obj, "opengl.faces&wireframe", faces);
	json_object_dotset_boolean(root_obj, "opengl.normals", render_normals);
	json_object_dotset_number(root_obj, "opengl.normal_length", normal_length);

	json_serialize_to_file(root_value, "config_data.json");

	ret = true;
	return ret;
}