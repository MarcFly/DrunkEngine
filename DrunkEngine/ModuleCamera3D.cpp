#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleUI.h"
#include "ConsoleWindow.h"
#include "GeoPropertiesWindow.h"
#include "ComponentCamera.h"

#define MOV_SPEED 4.0f
#define MOUSE_SENSIBILITY 0.01f
#define MOUSE_WHEEL_SPEED 6.0f

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	background = Color(0.1f, 0.1f, 0.1f, 1.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	main_camera = new ComponentCamera(nullptr);

	App->ui->console_win->AddLog("Setting up the camera");
	bool ret = true;	

	main_camera->Move(vec(10.0f, 10.0f, 5.0f));
	main_camera->LookAt(vec(0.0f, 0.0f, 0.0f));

	win_w = App->window->window_w;
	win_h = App->window->window_h;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	PLOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	if(ImGui::IsMouseHoveringAnyWindow())
		return UPDATE_CONTINUE;

	vec newPos(0, 0, 0);
	float speed = MOV_SPEED * dt * main_camera->mesh_multiplier;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = MOV_SPEED * 2 * dt * main_camera->mesh_multiplier;

	if (App->input->GetKey(App->input->controls[MOVE_FORWARD]) == KEY_REPEAT) newPos -= main_camera->Z * speed;
	if (App->input->GetKey(App->input->controls[MOVE_BACK]) == KEY_REPEAT) newPos += main_camera->Z * speed;

	if (App->input->GetKey(App->input->controls[MOVE_LEFT]) == KEY_REPEAT) newPos -= main_camera->X * speed;
	if (App->input->GetKey(App->input->controls[MOVE_RIGHT]) == KEY_REPEAT) newPos += main_camera->X * speed;

	if (App->input->GetMouseZ() < 0) newPos += main_camera->Z * speed * MOUSE_WHEEL_SPEED;
	if (App->input->GetMouseZ() > 0) newPos -= main_camera->Z * speed * MOUSE_WHEEL_SPEED;

	if (App->input->GetKey(App->input->controls[FOCUS_CAMERA]) == KEY_DOWN)
	{
		vec aux = App->mesh_loader->Root_Object->getObjectCenter();
		main_camera->LookAt(vec(aux.x,aux.y,aux.z)); // TODO Change to selected obj for assignment 2
	}
	main_camera->Position += newPos;

	if (App->input->GetKey(App->input->controls[ORBIT_CAMERA]) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		if (App->mesh_loader->Root_Object != nullptr)
			main_camera->Reference = vec(0.0f, 0.0f, 0.0f);
		else
		{
			vec aux = App->mesh_loader->Root_Object->getObjectCenter();
			main_camera->Reference = {aux.x, aux.y, aux.z}; // TODO Change to selected obj for assignment 2
		}
		main_camera->Rotate();
	}
	else
	{
		main_camera->Reference = main_camera->Position;
		main_camera->Reference += newPos;
	}
	// Mouse motion ----------------
	// TODO: Requires mouse reset properly without affecting MouseMotion
	// if want to get camera rotation without pressing Right Mouse Button
	// Or move mouse around the render windows while you are pressing then be able to continously drag around


	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		main_camera->Rotate();

	// Recalculate matrix -------------
	main_camera->CalculateViewMatrix();

	SDL_GetWindowSize(App->window->window, &win_w, &win_h);
	if (win_w != App->window->window_w || win_h != App->window->window_h)
	{
		App->window->window_w = win_w;
		App->window->window_h = win_h;
		main_camera->SetAspectRatio();
		App->renderer3D->OnResize();
	}

	return UPDATE_CONTINUE;
}

bool ModuleCamera3D::Load(JSON_Value * root_value)
{
	return false;
}

bool ModuleCamera3D::Save(JSON_Value* root_value)
{
	bool ret = false;

	root_value = json_parse_file("config_data.json");
	JSON_Object* root_obj = json_value_get_object(root_value);

	json_object_dotset_number(root_obj, "camera.pos.x", main_camera->X.x);
	json_object_dotset_number(root_obj, "camera.pos.y", main_camera->X.x);
	json_object_dotset_number(root_obj, "camera.pos.z", main_camera->X.x);

	json_serialize_to_file(root_value, "config_data.json");

	App->ui->console_win->AddLog("Camera position saved");

	ret = true;
	return ret;
}