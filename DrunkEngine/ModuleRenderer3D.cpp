#include "Application.h"
#include "ModuleRenderer3D.h"
#include "PhysBody3D.h"
#include "ModuleScene.h"
#include "ModuleUI.h"
#include "ConsoleWindow.h"
#include "GameObject.h"
#include "ComponentCamera.h"

#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#define GRID_SIZE 10

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled, Type_Render3D)
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

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	//Create context
	App->ui->console_win->AddLog("Creating 3D Renderer context");
	context = SDL_GL_CreateContext(App->window->window);
	if (GLEW_OK != glewInit())
	{
		App->ui->console_win->AddLog("Failed GLEW Initiation!\n");
	}

	if(context == NULL)
	{
		App->ui->console_win->AddLog("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	//Load(nullptr);

	if(ret == true)
	{
		//Use Vsync
		if(SDL_GL_SetSwapInterval(vsync) < 0)
			App->ui->console_win->AddLog("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

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
	OnResize();
	
	SetTextureParams();
	
	App->eventSys->Subscribe(EventType::Window_Resize, this);
	App->eventSys->Subscribe(EventType::Camera_Modified, this);

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	Color c = App->camera->background;
	glClearColor(c.r,c.g,c.b,c.a);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//int width, height;
	//SDL_GetWindowSize(App->window->window, &width, &height);
	//glViewport(0, 0, width, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->scene->Main_Cam->GetViewMatrix());

	RenderGrid();

	// Something Something lights
	// Set light pos
	lights[0].SetPos(App->scene->Main_Cam->frustum.pos.x, App->scene->Main_Cam->frustum.pos.y, App->scene->Main_Cam->frustum.pos.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// Do the render of Objects
update_status ModuleRenderer3D::Update(float dt)
{
	App->scene->ObjUpdate(dt);
	/*if (faces)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		Render(true);
	}

	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0, 0, 0);
		Render(false);
	}*/

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
	App->ui->console_win->AddLog("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::Render(bool use_texture)
{
	/*
	for (int i = 0; i < App->scene->Objects.size(); i++)
	{
		for(int j = 0; j < App->scene->Objects[i].meshes.size(); j++)
		{ 
			// Draw elements
			ComponentMesh* mesh = &App->scene->Objects[i].meshes[j];

			if (faces)
				App->scene->DrawMesh(mesh, use_texture);

			// Draw Normals
			if (render_normals)
			{
				glBegin(GL_LINES);
				glColor3f(0.0f, 1.0f, 0.0f);

				for (int k = 0; k < mesh->num_normal / 6; k++)
				{
					glVertex3f(mesh->normal[k * 6], mesh->normal[k * 6 + 1], mesh->normal[k * 6 + 2]);

					vec norm(mesh->normal[k * 6 + 3] - mesh->normal[k * 6], mesh->normal[k * 6 + 4] - mesh->normal[k * 6 + 1], mesh->normal[k * 6 + 5] - mesh->normal[k * 6 + 2]);
					norm = norm.Mul(normal_length);

					glVertex3f(mesh->normal[k * 6] + norm.x, mesh->normal[k * 6 + 1] + norm.y, mesh->normal[k * 6 + 2] + norm.z);
				}

				glEnd();
			}

			glDisableClientState(GL_VERTEX_ARRAY);

			if (bounding_box)
				RenderBoundBox(mesh);
		}

		
	}
	*/
}

void ModuleRenderer3D::OnResize()
{
	glViewport(0, 0, App->window->window_w, App->window->window_h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//From cameracomp
	if (App->scene->Main_Cam != nullptr)
		ProjectionMatrix = App->scene->Main_Cam->frustum.ProjectionMatrix();
	//ProjectionMatrix = ProjectionMatrix.perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);

	glLoadMatrixf(&ProjectionMatrix[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::ChangeVsync()
{
	if (SDL_GL_SetSwapInterval(vsync) < 0)
		App->ui->console_win->AddLog("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
}

bool ModuleRenderer3D::CheckGLError()
{
	bool ret = true;

	//Check for error
	if (glGetError() != GL_NO_ERROR)
	{
		App->ui->console_win->AddLog("Error initializing OpenGL! %s\n", gluErrorString(glGetError()));
		ret = false;
	}

	return ret;
}

void ModuleRenderer3D::RenderGrid()
{
	glDisable(GL_LIGHTING);

	for (int i = 0; i < GRID_SIZE * 2 + 1; i++)
	{
		glBegin(GL_LINES);
		glColor3f(0.5f, 0.5f, 0.5f);

		//Z
		glVertex3i(GRID_SIZE - i, 0, GRID_SIZE);
		glVertex3i(GRID_SIZE - i, 0, -GRID_SIZE);

		//X
		glVertex3i(-GRID_SIZE, 0, -GRID_SIZE + i);
		glVertex3i(GRID_SIZE, 0, -GRID_SIZE + i);
		glEnd();
	}

	if (lighting)
		glEnable(GL_LIGHTING);
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

void ModuleRenderer3D::RecieveEvent(const Event & event)
{
	switch (event.type)
	{
	case EventType::Camera_Modified:
	{
		OnResize();
		break;
	}
	case EventType::Window_Resize:
	{
		OnResize();
		break;
	}
	default:
		break;
	}
}

void ModuleRenderer3D::SetTextureParams()
{
	switch (curr_tws) {
	case (TP_CLAMP_TO_EDGE - TP_TEXTURE_WRAP - 1): tws = GL_CLAMP_TO_EDGE;	break;
	case (TP_CLAMP_TO_BORDER - TP_TEXTURE_WRAP - 1): tws = GL_CLAMP_TO_BORDER;	break;
	case (TP_MIRRORED_REPEAT - TP_TEXTURE_WRAP - 1): tws = GL_MIRRORED_REPEAT; break;
	case (TP_REPEAT - TP_TEXTURE_WRAP - 1):	tws = GL_REPEAT; break;
	case (TP_MIRROR_CLAMP_TO_EDGE - TP_TEXTURE_WRAP - 1): tws = GL_MIRROR_CLAMP_TO_EDGE; break;
	default: App->ui->console_win->AddLog("Unsuccessful initialization");
	}

	switch (curr_twt) {
	case (TP_CLAMP_TO_EDGE - TP_TEXTURE_WRAP - 1): twt = GL_CLAMP_TO_EDGE; break;
	case (TP_CLAMP_TO_BORDER - TP_TEXTURE_WRAP - 1): twt = GL_CLAMP_TO_BORDER; break;
	case (TP_MIRRORED_REPEAT - TP_TEXTURE_WRAP - 1): twt = GL_MIRRORED_REPEAT; break;
	case (TP_REPEAT - TP_TEXTURE_WRAP - 1): twt = GL_REPEAT; break;
	case (TP_MIRROR_CLAMP_TO_EDGE - TP_TEXTURE_WRAP - 1): twt = GL_MIRROR_CLAMP_TO_EDGE; break;
	default: App->ui->console_win->AddLog("Unsuccessful initialization");
	}

	// Texture Filter
	switch (curr_tmagf) {
	case (TP_NEAREST - TP_TEXTURE_FILTERS - 1): tmagf = GL_LINEAR; break;
	case (TP_LINEAR - TP_TEXTURE_FILTERS - 1): tmagf = GL_NEAREST; break;
	case (TP_NEAREST_MIPMAP_NEAREST - TP_TEXTURE_FILTERS - 1): tmagf = GL_NEAREST_MIPMAP_NEAREST; break;
	case (TP_LINEAR_MIPMAP_NEAREST - TP_TEXTURE_FILTERS - 1): tmagf = GL_LINEAR_MIPMAP_NEAREST; break;
	case (TP_NEAREST_MIPMAP_LINEAR - TP_TEXTURE_FILTERS - 1): tmagf = GL_NEAREST_MIPMAP_LINEAR; break;
	case (TP_LINEAR_MIPMAP_LINEAR - TP_TEXTURE_FILTERS - 1): tmagf = GL_LINEAR_MIPMAP_LINEAR; break;
	default: App->ui->console_win->AddLog("Unsuccessful initialization");
	}

	switch (curr_tminf) {
	case (TP_NEAREST - TP_TEXTURE_FILTERS - 1): tminf = GL_LINEAR; break;
	case (TP_LINEAR - TP_TEXTURE_FILTERS - 1): tminf = GL_NEAREST; break;
	case (TP_NEAREST_MIPMAP_NEAREST - TP_TEXTURE_FILTERS - 1): tminf = GL_NEAREST_MIPMAP_NEAREST; break;
	case (TP_LINEAR_MIPMAP_NEAREST - TP_TEXTURE_FILTERS - 1): tminf = GL_LINEAR_MIPMAP_NEAREST; break;
	case (TP_NEAREST_MIPMAP_LINEAR - TP_TEXTURE_FILTERS - 1): tminf = GL_NEAREST_MIPMAP_LINEAR; break;
	case (TP_LINEAR_MIPMAP_LINEAR - TP_TEXTURE_FILTERS - 1): tminf = GL_LINEAR_MIPMAP_LINEAR; break;
	default: App->ui->console_win->AddLog("Unsuccessful initialization");
	}
}

void ModuleRenderer3D::GenTexParams()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tws);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, twt);

	// Texture Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tmagf);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tminf);
}

// SAVELOAD--------------------------------------------------------------------------------
bool ModuleRenderer3D::Load(JSON_Value * root_value)
{
	bool ret = false;

	root_value = json_parse_file("config_data.json");

	depth_test = json_object_dotget_boolean(json_object(root_value), "render.depth_test");
	cull_face = json_object_dotget_boolean(json_object(root_value), "render.cull_face");
	lighting = json_object_dotget_boolean(json_object(root_value), "render.lighting");
	color_material = json_object_dotget_boolean(json_object(root_value), "render.color_materials");
	texture_2d = json_object_dotget_boolean(json_object(root_value), "render.texture2d");
	wireframe = json_object_dotget_boolean(json_object(root_value), "render.wireframe");
	faces = json_object_dotget_boolean(json_object(root_value), "render.faces&wireframe");
	render_normals = json_object_dotget_boolean(json_object(root_value), "render.normals");
	normal_length = json_object_dotget_number(json_object(root_value), "render.normal_length");
	vsync = json_object_dotget_boolean(json_object(root_value), "render.vsync");
	bounding_box = json_object_dotget_boolean(json_object(root_value), "render.bounding_box");

	curr_tws = json_object_dotget_number(json_object(root_value), "render.curr_wrap_s");
	curr_twt = json_object_dotget_number(json_object(root_value), "render.curr_wrap_t");
	curr_tmagf = json_object_dotget_number(json_object(root_value), "render.curr_min_filter");
	curr_tminf = json_object_dotget_number(json_object(root_value), "render.curr_mag_filter");


	ret = true;
	return ret;
}

bool ModuleRenderer3D::Save(JSON_Value * root_value)
{
	bool ret = false;

	JSON_Object* root_obj = json_value_get_object(root_value);

	json_object_dotset_boolean(root_obj, "render.depth_test", depth_test);
	json_object_dotset_boolean(root_obj, "render.cull_face", cull_face);
	json_object_dotset_boolean(root_obj, "render.lighting", lighting);
	json_object_dotset_boolean(root_obj, "render.color_materials", color_material);
	json_object_dotset_boolean(root_obj, "render.texture2d", texture_2d);
	json_object_dotset_boolean(root_obj, "render.wireframe", wireframe);
	json_object_dotset_boolean(root_obj, "render.faces&wireframe", faces);
	json_object_dotset_boolean(root_obj, "render.normals", render_normals);
	json_object_dotset_number(root_obj, "render.normal_length", normal_length);
	json_object_dotset_boolean(root_obj, "render.vsync", vsync);
	json_object_dotset_boolean(root_obj, "render.bounding_box", bounding_box);

	json_object_dotset_number(root_obj, "render.curr_wrap_s", curr_tws);
	json_object_dotset_number(root_obj, "render.curr_wrap_t", curr_twt);
	json_object_dotset_number(root_obj, "render.curr_min_filter", curr_tmagf);
	json_object_dotset_number(root_obj, "render.curr_mag_filter", curr_tminf);

	json_serialize_to_file(root_value, "config_data.json");

	//App->ui->console_win->AddLog("Render config saved");

	ret = true;
	return ret;
}