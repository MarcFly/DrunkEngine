#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	PLOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		PLOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		int width = SCREEN_WIDTH * SCREEN_SIZE;
		int height = SCREEN_HEIGHT * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			PLOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	PLOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

bool ModuleWindow::Load(JSON_Value* root_value)
{
	bool ret = false;

	root_value = json_parse_file("config_data.json");
	SDL_SetWindowSize(window, json_object_dotget_number(json_object(root_value), "window.size.width"), json_object_dotget_number(json_object(root_value), "window.size.height"));

	ret = true;
	return ret;
}

bool ModuleWindow::Save(JSON_Value* root_value)
{
	bool ret = false;

	//JSON_Value* window_value = root_value;
	//JSON_Value *schema = json_parse_string("{\"width\:\"\"height\":\"\"}");
	root_value = json_parse_file("config_data.json");
	//root_value = json_value_init_object();
	JSON_Object* root_obj = json_value_get_object(root_value);
	
	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	json_object_dotset_number(root_obj, "window.size.width", width);
	//json_object_set_number(root_obj, "width", width);
	json_object_dotset_number(root_obj, "window.size.height", height);
	json_serialize_to_file(root_value, "config_data.json");

	ret = true;
	return ret;
}