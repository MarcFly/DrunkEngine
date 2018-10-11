#include "Application.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
#include "ModuleUI.h"
#include "ConsoleWindow.h"

#define MOV_SPEED 4.0f
#define MOUSE_SENSIBILITY 0.2f
#define MOUSE_WHEEL_SPEED 6.0f

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = Position;

	background = Color(0.1f, 0.1f, 0.1f, 1.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	App->ui->console_win->AddLog("Setting up the camera");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	LookAt(vec3(0.0f, 0.0f, 0.0f));

	mesh_multiplier = 1;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	App->ui->console_win->AddLog("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	vec3 newPos(0, 0, 0);
	float speed = MOV_SPEED * dt * mesh_multiplier;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = MOV_SPEED * 2 * dt * mesh_multiplier;

	if (App->input->GetKey(App->input->controls[MOVE_FORWARD]) == KEY_REPEAT) newPos -= Z * speed;
	if (App->input->GetKey(App->input->controls[MOVE_BACK]) == KEY_REPEAT) newPos += Z * speed;

	if (App->input->GetKey(App->input->controls[MOVE_LEFT]) == KEY_REPEAT) newPos -= X * speed;
	if (App->input->GetKey(App->input->controls[MOVE_RIGHT]) == KEY_REPEAT) newPos += X * speed;

	if (App->input->GetMouseZ() < 0) newPos += Z * speed * MOUSE_WHEEL_SPEED;
	if (App->input->GetMouseZ() > 0) newPos -= Z * speed * MOUSE_WHEEL_SPEED;

	if (App->input->GetKey(App->input->controls[FOCUS_CAMERA]) == KEY_DOWN) LookAt(vec3(0.0f, 0.0f, 0.0f));

	Position += newPos;

	if (App->input->GetKey(App->input->controls[ORBIT_CAMERA]) == KEY_REPEAT)
		Reference = vec3(0.0f, 0.0f, 0.0f);
	else
	{
		Reference = Position;
		Reference += newPos;
	}
	// Mouse motion ----------------
	// TODO: Requires mouse reset properly without affecting MouseMotion
	// if want to get camera rotation without pressing Right Mouse Button
	// Or move mouse around the render windows while you are pressing then be able to continously drag around


	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		Position -= Reference;

		if (dx != 0)
		{
			float DeltaX = (float)dx * MOUSE_SENSIBILITY;

			X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));

		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * MOUSE_SENSIBILITY;

			Y = rotate(Y, DeltaY, X);
			Z = rotate(Z, DeltaY, X);

			if (Y.y < 0.0f)
			{
				Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = cross(Z, X);
			}

		}

		Position = Reference + Z * length(Position);
	}

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}


	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::Transport(const vec3 &Movement)
{
	Position = Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return (float*)ViewMatrix.v;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);

	ViewMatrixInverse = ViewMatrix.Inverted();
}

bool ModuleCamera3D::Load(JSON_Value* root_value)
{
	bool ret = false;

	root_value = json_parse_file("config_data.json");
	//SDL_SetWindowSize(window, json_object_dotget_number(json_object(root_value), "window.size.width"), json_object_dotget_number(json_object(root_value), "window.size.height"));
	Look({ (float)json_object_dotget_number(json_object(root_value), "camera.pos.x") ,(float)json_object_dotget_number(json_object(root_value), "camera.pos.y") ,(float)json_object_dotget_number(json_object(root_value), "camera.pos.z") }, { 0,0,0 });

	ret = true;
	return ret;
}

bool ModuleCamera3D::Save(JSON_Value* root_value)
{
	bool ret = false;

	//JSON_Value* window_value = root_value;
	//JSON_Value *schema = json_parse_string("{\"width\:\"\"height\":\"\"}");
	root_value = json_parse_file("config_data.json");
	//root_value = json_value_init_object();
	JSON_Object* root_obj = json_value_get_object(root_value);

	//SDL_GetWindowSize(window, &width, &height);
	json_object_dotset_number(root_obj, "camera.pos.x", X.x);
	json_object_dotset_number(root_obj, "camera.pos.y", X.x);
	json_object_dotset_number(root_obj, "camera.pos.z", X.x);

	json_serialize_to_file(root_value, "config_data.json");

	ret = true;
	return ret;
}