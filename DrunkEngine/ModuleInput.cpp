#include "Application.h"
#include "ModuleInput.h"
#include "ModuleUI.h"
#include "ModuleScene.h"
#include "OptionsWindow.h"
#include "AboutWindow.h"
#include "ConsoleWindow.h"
#include "SceneViewerWindow.h"
#include "KdTreeWindow.h"
#include "InspectorWindow.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(bool start_enabled) : Module(start_enabled, Type_Input)
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
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					int win_w, win_h;
					SDL_GetWindowSize(App->window->window, &win_w, &win_h);

					Event ev(EventType::Window_Resize, Event::UnionUsed::UsePoint2d);
					ev.point2d.x = win_w;
					ev.point2d.y = win_h;
					App->eventSys->BroadcastEvent(ev);
				}
				break;
			}
			
			/*
			case SDL_DROPFILE:		// In case if dropped file
			{      
				char* dropped_filedir = nullptr;
				
				dropped_filedir = e.drop.file;
				// Shows directory of dropped file

				App->importer->LoadFile(dropped_filedir);
				
				SDL_free(dropped_filedir);
				break;
			}
			*/
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
	App->ui->options_win->SetShortCut((SDL_Scancode)menu_c[OPTIONS_MENU]);
	App->ui->console_win->SetShortCut((SDL_Scancode)menu_c[CONSOLE_MENU]);
	App->ui->scene_viewer_window->SetShortCut((SDL_Scancode)menu_c[INSPECTOR]);
	App->ui->about_win->SetShortCut((SDL_Scancode)menu_c[ABOUT_MENU]);
	App->ui->scene_viewer_window->SetShortCut((SDL_Scancode)menu_c[SCENE_VIEWER_MENU]);
	App->ui->kdtree_win->SetShortCut((SDL_Scancode)menu_c[KD_TREE_MENU]);
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
	menu_c[OPTIONS_MENU] = SDL_SCANCODE_O;
	menu_c[CONSOLE_MENU] = SDL_SCANCODE_C;
	menu_c[INSPECTOR] = SDL_SCANCODE_I;
	menu_c[ABOUT_MENU] = SDL_SCANCODE_A;
	menu_c[SCENE_VIEWER_MENU] = SDL_SCANCODE_V;
	menu_c[KD_TREE_MENU] = SDL_SCANCODE_T;

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
	menu_c[OPTIONS_MENU] = json_object_dotget_number(json_object(root_value), "controls.options_menu");
	menu_c[CONSOLE_MENU] = json_object_dotget_number(json_object(root_value), "controls.console_menu");
	menu_c[INSPECTOR] = json_object_dotget_number(json_object(root_value), "controls.inspector_menu");
	menu_c[ABOUT_MENU] = json_object_dotget_number(json_object(root_value), "controls.about_menu");
	menu_c[SCENE_VIEWER_MENU] = json_object_dotget_number(json_object(root_value), "controls.scene_viewer_menu");
	menu_c[KD_TREE_MENU] = json_object_dotget_number(json_object(root_value), "controls.kd_tree_menu");

	ret = true;
	return ret;
}

bool ModuleInput::Save(JSON_Value * root_value)
{
	bool ret = false;

	JSON_Object* root_obj = json_value_get_object(root_value);

	//Controls
	json_object_dotset_number(root_obj, "controls.move_forward", controls[MOVE_FORWARD]);
	json_object_dotset_number(root_obj, "controls.move_back", controls[MOVE_BACK]);
	json_object_dotset_number(root_obj, "controls.move_left", controls[MOVE_LEFT]);
	json_object_dotset_number(root_obj, "controls.move_right", controls[MOVE_RIGHT]);
	json_object_dotset_number(root_obj, "controls.focus_camera", controls[FOCUS_CAMERA]);
	json_object_dotset_number(root_obj, "controls.orbit_camera", controls[ORBIT_CAMERA]);

	//Menus
	json_object_dotset_number(root_obj, "controls.options_menu", menu_c[OPTIONS_MENU]);
	json_object_dotset_number(root_obj, "controls.console_menu", menu_c[CONSOLE_MENU]);
	json_object_dotset_number(root_obj, "controls.inspector_menu", menu_c[INSPECTOR]);
	json_object_dotset_number(root_obj, "controls.about_menu", menu_c[ABOUT_MENU]);
	json_object_dotset_number(root_obj, "controls.scene_viewer_menu", menu_c[SCENE_VIEWER_MENU]);
	json_object_dotset_number(root_obj, "controls.kd_tree_menu", menu_c[KD_TREE_MENU]);
	

	json_serialize_to_file(root_value, "config_data.json");

	//App->ui->console_win->AddLog("Input config saved");

	ret = true;
	return ret;
}