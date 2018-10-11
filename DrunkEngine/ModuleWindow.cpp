#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleUI.h"
#include "ConsoleWindow.h"

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

		// Set Important SDL_GL_Attributes
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,1);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);

		Load(nullptr);

		if(fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(full_desktop == true)
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
	App->ui->console_win->AddLog("Destroying SDL window and quitting all SDL systems");

	// Destroy Surface
	if (screen_surface != NULL)
	{
		SDL_FreeSurface(screen_surface);
		screen_surface = NULL;
	}

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
		window = NULL;
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

void ModuleWindow::SetFullscreen(bool fullscreen)
{
	if (fullscreen)
	{
		SDL_DisplayMode DM;
		SDL_GetCurrentDisplayMode(0, &DM);
		SDL_GetWindowSize(window, &window_w, &window_h);
		SDL_SetWindowSize(window, DM.w, DM.h);

		SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN);
	}
	else
	{
		SDL_SetWindowFullscreen(this->window, 0);
		SDL_SetWindowSize(window, window_w, window_h);
	}
}

void ModuleWindow::SetResizable(bool resizable)
{
	SDL_SetWindowResizable(this->window, (SDL_bool)this->resizable);
}

void ModuleWindow::SetBorderless(bool borderless)
{
	SDL_SetWindowBordered(App->window->window, (SDL_bool)!this->borderless);
}

void ModuleWindow::SetFullDesktop(bool full_desktop)
{
	if (full_desktop)
		SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(this->window, 0);
}

void ModuleWindow::SetBrightness(float brightness)
{
	SDL_SetWindowBrightness(this->window, this->brightness);
}


// SAVE & LOAD ----------------------------------------------------------------------------------------
bool ModuleWindow::Load(JSON_Value* root_value)
{
	bool ret = false;

	root_value = json_parse_file("config_data.json");
	window_w = json_object_dotget_number(json_object(root_value), "window.size.width");
	window_h = json_object_dotget_number(json_object(root_value), "window.size.height");
	SDL_SetWindowSize(window, window_w, window_h);

	fullscreen = json_object_dotget_boolean(json_object(root_value), "window.options.fullscreen");
	SetFullscreen(fullscreen);

	resizable = json_object_dotget_boolean(json_object(root_value), "window.options.resizable");
	SetResizable(resizable);

	borderless = json_object_dotget_boolean(json_object(root_value), "window.options.borderless");
	SetBorderless(borderless);

	full_desktop = json_object_dotget_boolean(json_object(root_value), "window.options.full_desktop");
	SetFullDesktop(full_desktop);

	brightness = json_object_dotget_number(json_object(root_value), "window.options.brightness");
	SetBrightness(brightness);

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
	
	json_object_dotset_number(root_obj, "window.size.width", window_w);
	//json_object_set_number(root_obj, "width", width);
	json_object_dotset_number(root_obj, "window.size.height", window_h);
	
	json_object_dotset_boolean(root_obj, "window.options.fullscreen", this->fullscreen);
	json_object_dotset_boolean(root_obj, "window.options.resizable", this->resizable);
	json_object_dotset_boolean(root_obj, "window.options.borderless", this->borderless);
	json_object_dotset_boolean(root_obj, "window.options.full_desktop", this->full_desktop);
	json_object_dotset_number(root_obj, "window.options.brightness", this->brightness);

	json_serialize_to_file(root_value, "config_data.json");

	ret = true;
	return ret;
}