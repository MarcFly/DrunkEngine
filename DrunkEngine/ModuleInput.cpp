#include "Application.h"
#include "ModuleInput.h"
#include "ModuleUI.h"
#include "ModuleManageMesh.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init()
{
	PLOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		PLOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	quit = false;

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetGlobalMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		App->ui->ProcessEvent(&e);

		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			break;

			case SDL_MOUSEMOTION:
			mouse_x = e.motion.x / SCREEN_SIZE;
			mouse_y = e.motion.y / SCREEN_SIZE;

			mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
			break;

			case SDL_QUIT:
			quit = true;
			break;

			case SDL_WINDOWEVENT:
			{
				if(e.window.event == SDL_WINDOWEVENT_RESIZED)
					App->renderer3D->OnResize(e.window.data1, e.window.data2);
				break;
			}

			case SDL_DROPFILE:		// In case if dropped file
			{      
				char* dropped_filedir = nullptr;
				
				dropped_filedir = e.drop.file;
				// Shows directory of dropped file


				SDL_ShowSimpleMessageBox(
					SDL_MESSAGEBOX_INFORMATION,
					"File dropped on window",
					dropped_filedir,
					App->window->window);

				std::string extension = strchr(dropped_filedir, '.');
				if (extension == std::string(".fbx") || extension == std::string(".FBX"))
					App->mesh_loader->LoadFBX(dropped_filedir);
				else if (App->mesh_loader->Objects.size() > 0) // In case we have no objects
				{
					if (extension == std::string(".png") || extension == std::string(".PNG"))
						App->mesh_loader->LoadTextCurrentObj(dropped_filedir, &App->mesh_loader->Objects[App->mesh_loader->Objects.size() -1]);
					else if (extension == std::string(".bmp") || extension == std::string(".BMP"))
						App->mesh_loader->LoadTextCurrentObj(dropped_filedir, &App->mesh_loader->Objects[0]);
					else if (extension == std::string(".jpg") || extension == std::string(".JPG"))
						App->mesh_loader->LoadTextCurrentObj(dropped_filedir, &App->mesh_loader->Objects[0]);
					else if (extension == std::string(".dds") || extension == std::string(".DDS"))
						App->mesh_loader->LoadTextCurrentObj(dropped_filedir, &App->mesh_loader->Objects[0]);
				}
				else
					PLOG("File format not recognized!\n");

				SDL_free(dropped_filedir);
				break;
			}
		}
	}

	if(quit == true)
		return UPDATE_STOP;



	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	PLOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}