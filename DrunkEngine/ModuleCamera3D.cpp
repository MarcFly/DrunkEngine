#include "Application.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
#include "ModuleUI.h"
#include "ConsoleWindow.h"
#include "GeoPropertiesWindow.h"

#define MOV_SPEED 4.0f
#define MOUSE_SENSIBILITY 0.2f
#define MOUSE_WHEEL_SPEED 6.0f

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	CalculateViewMatrix();

	X = vec(1.0f, 0.0f, 0.0f);
	Y = vec(0.0f, 1.0f, 0.0f);
	Z = vec(0.0f, 0.0f, 1.0f);

	Position = vec(0.0f, 0.0f, 5.0f);
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

	App->camera->Move(vec(1.0f, 1.0f, 0.0f));
	LookAt(vec(0.0f, 0.0f, 0.0f));

	mesh_multiplier = 1;

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
	float speed = MOV_SPEED * dt * mesh_multiplier;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = MOV_SPEED * 2 * dt * mesh_multiplier;

	if (App->input->GetKey(App->input->controls[MOVE_FORWARD]) == KEY_REPEAT) newPos -= Z * speed;
	if (App->input->GetKey(App->input->controls[MOVE_BACK]) == KEY_REPEAT) newPos += Z * speed;

	if (App->input->GetKey(App->input->controls[MOVE_LEFT]) == KEY_REPEAT) newPos -= X * speed;
	if (App->input->GetKey(App->input->controls[MOVE_RIGHT]) == KEY_REPEAT) newPos += X * speed;

	if (App->input->GetMouseZ() < 0) newPos += Z * speed * MOUSE_WHEEL_SPEED;
	if (App->input->GetMouseZ() > 0) newPos -= Z * speed * MOUSE_WHEEL_SPEED;

	if (App->input->GetKey(App->input->controls[FOCUS_CAMERA]) == KEY_DOWN)
	{
		vec aux = App->mesh_loader->Root_Object->getObjectCenter();
		LookAt(vec(aux.x,aux.y,aux.z)); // TODO Change to selected obj for assignment 2
	}
	Position += newPos;

	if (App->input->GetKey(App->input->controls[ORBIT_CAMERA]) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		if (App->mesh_loader->Root_Object != nullptr)
			Reference = vec(0.0f, 0.0f, 0.0f);
		else
		{
			vec aux = App->mesh_loader->Root_Object->getObjectCenter();
			Reference = {aux.x, aux.y, aux.z}; // TODO Change to selected obj for assignment 2
		}
		Rotate();
	}
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
		Rotate();

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec &Position, const vec &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	float3 aux = Position - Reference;
	Z = aux.Normalized();
	aux = float3(0.0f, 1.0f, 0.0f).Cross(Z);
	X = aux.Normalized();
	Y = Z.Cross(X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}


	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const vec &Spot)
{
	Reference = Spot;

	float3 aux = Position - Reference;
	Z = aux.Normalized();
	aux = float3(0.0f, 1.0f, 0.0f).Cross(Z);
	X = aux.Normalized();
	Y = Z.Cross(X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::Transport(const vec &Movement)
{
	Position = Movement;

	CalculateViewMatrix();
}

void ModuleCamera3D::Rotate()
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	if (dx != 0)
	{
		float DeltaX = (float)dx * MOUSE_SENSIBILITY;

		X = RotateAngle(X, DeltaX, float3(0.0f, 1.0f, 0.0f));
		Y = RotateAngle(Y, DeltaX, float3(0.0f, 1.0f, 0.0f));
		Z = RotateAngle(Z, DeltaX, float3(0.0f, 1.0f, 0.0f));

	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * MOUSE_SENSIBILITY;

		Y = RotateAngle(Y, DeltaY, X);
		Z = RotateAngle(Z, DeltaY, X);

		if (Y.y < 0.0f)
		{
			Z = float3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = Z.Cross(X);
		}

	}

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		Position -= Reference;
		Position = Reference + Z * Position.Length();
	}
}

float3 ModuleCamera3D::RotateAngle(const float3 &u, float angle, const float3 &v)
{
	return *(float3*)&(float4x4::RotateAxisAngle(v, angle) * float4(u, 1.0f));
}
// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix[0][0];
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -X.Dot(Position), -Y.Dot(Position), -Z.Dot(Position), 1.0f);

	ViewMatrixInverse = ViewMatrix.Inverted();
}

bool ModuleCamera3D::Load(JSON_Value* root_value)
{
	bool ret = false;

	root_value = json_parse_file("config_data.json");
	Look({ (float)json_object_dotget_number(json_object(root_value), "camera.pos.x") ,(float)json_object_dotget_number(json_object(root_value), "camera.pos.y") ,(float)json_object_dotget_number(json_object(root_value), "camera.pos.z") }, { 0,0,0 });

	ret = true;
	return ret;
}

bool ModuleCamera3D::Save(JSON_Value* root_value)
{
	bool ret = false;

	root_value = json_parse_file("config_data.json");
	JSON_Object* root_obj = json_value_get_object(root_value);

	json_object_dotset_number(root_obj, "camera.pos.x", X.x);
	json_object_dotset_number(root_obj, "camera.pos.y", X.x);
	json_object_dotset_number(root_obj, "camera.pos.z", X.x);

	json_serialize_to_file(root_value, "config_data.json");

	App->ui->console_win->AddLog("Camera position saved");

	ret = true;
	return ret;
}

void ModuleCamera3D::SetToObj(GameObject* obj, float vertex_aux)
{
	
	//for (int i = 0; i < obj->meshes.size() - 1; i++) {
	//	for (uint j = 0; j < obj->meshes[i].num_vertex * 3; j++)
	//	{
	//		if (vertex_aux < abs(obj->meshes[i].vertex[j]))
	//			vertex_aux = abs(obj->meshes[i].vertex[j]);
	//	}
	//}

	Transport(vec(vertex_aux + 3, vertex_aux + 3, vertex_aux + 3));

	vec aux = obj->getObjectCenter();
	LookAt(vec(aux.x, aux.y, aux.z));

	mesh_multiplier = vertex_aux / 4;
}