#include "Application.h"
#include "ModuleInput.h"
#include "ModuleUI.h"
#include "ModuleScene.h"

#include "OptionsWindow.h"
#include "AboutWindow.h"
#include "RandomGenWindow.h"
#include "GeometryCreationWindow.h"
#include "ConsoleWindow.h"
#include "GeoPropertiesWindow.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(bool start_enabled) : Module(start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	if(keyboard != nullptr)
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

	//DefaultControls();
	Load(nullptr);

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

	//mouse_x /= SCREEN_SIZE;
	//mouse_y /= SCREEN_SIZE;
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
				mouse_x = e.motion.x; // / SCREEN_SIZE;
				mouse_y = e.motion.y; // / SCREEN_SIZE;

				mouse_x_motion = e.motion.xrel; // / SCREEN_SIZE;
				mouse_y_motion = e.motion.yrel; // / SCREEN_SIZE;
			break;

			case SDL_QUIT:
				quit = true;;
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

				App->importer->LoadFile(dropped_filedir);
				
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

	if(keyboard != nullptr)
		delete[] keyboard;
	keyboard = nullptr;

	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

void ModuleInput::UpdateShortcuts()
{
	App->ui->options_win->SetShortCut((SDL_Scancode)controls[OPTIONS_MENU]);
	App->ui->console_win->SetShortCut((SDL_Scancode)controls[CONSOLE_MENU]);
	App->ui->geo_properties_win->SetShortCut((SDL_Scancode)controls[MESH_MENU]);
	App->ui->about_win->SetShortCut((SDL_Scancode)controls[ABOUT_MENU]);
}

void ModuleInput::SetDefaultControls()
{
	//Camera controls
	controls[MOVE_FORWARD] = SDL_SCANCODE_W;
	controls[MOVE_BACK] = SDL_SCANCODE_S;
	controls[MOVE_LEFT] = SDL_SCANCODE_A;
	controls[MOVE_RIGHT] = SDL_SCANCODE_D;
	controls[FOCUS_CAMERA] = SDL_SCANCODE_F;
	controls[ORBIT_CAMERA] = SDL_SCANCODE_LALT;

	//Menu Shortcuts
	controls[OPTIONS_MENU] = SDL_SCANCODE_O;
	controls[CONSOLE_MENU] = SDL_SCANCODE_C;
	controls[MESH_MENU] = SDL_SCANCODE_M;
	controls[ABOUT_MENU] = SDL_SCANCODE_I;

	App->ui->console_win->AddLog("Input config set to default");
}

bool ModuleInput::Load(JSON_Value * root_value)
{
	bool ret = false;

	root_value = json_parse_file("config_data.json");
	
	//Camera controls
	controls[MOVE_FORWARD] = json_object_dotget_number(json_object(root_value), "controls.move_forward");
	controls[MOVE_BACK] = json_object_dotget_number(json_object(root_value), "controls.move_back");
	controls[MOVE_LEFT] = json_object_dotget_number(json_object(root_value), "controls.move_left");
	controls[MOVE_RIGHT] = json_object_dotget_number(json_object(root_value), "controls.move_right");
	controls[FOCUS_CAMERA] = json_object_dotget_number(json_object(root_value), "controls.focus_camera");
	controls[ORBIT_CAMERA] = json_object_dotget_number(json_object(root_value), "controls.orbit_camera");

	//Menu Shortcuts
	controls[OPTIONS_MENU] = json_object_dotget_number(json_object(root_value), "controls.options_menu");
	controls[CONSOLE_MENU] = json_object_dotget_number(json_object(root_value), "controls.console_menu");
	controls[MESH_MENU] = json_object_dotget_number(json_object(root_value), "controls.mesh_menu");
	controls[ABOUT_MENU] = json_object_dotget_number(json_object(root_value), "controls.about_menu");

	ret = true;
	return ret;
}

bool ModuleInput::Save(JSON_Value * root_value)
{
	bool ret = false;


	root_value = json_parse_file("config_data.json");
	JSON_Object* root_obj = json_value_get_object(root_value);

	json_object_dotset_number(root_obj, "controls.move_forward", controls[MOVE_FORWARD]);
	json_object_dotset_number(root_obj, "controls.move_back", controls[MOVE_BACK]);
	json_object_dotset_number(root_obj, "controls.move_left", controls[MOVE_LEFT]);
	json_object_dotset_number(root_obj, "controls.move_right", controls[MOVE_RIGHT]);
	json_object_dotset_number(root_obj, "controls.focus_camera", controls[FOCUS_CAMERA]);
	json_object_dotset_number(root_obj, "controls.orbit_camera", controls[ORBIT_CAMERA]);
	json_object_dotset_number(root_obj, "controls.options_menu", controls[OPTIONS_MENU]);
	json_object_dotset_number(root_obj, "controls.mesh_menu", controls[MESH_MENU]);

	json_serialize_to_file(root_value, "config_data.json");

	App->ui->console_win->AddLog("Input config saved");

	ret = true;
	return ret;
}