#include "Application.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	CalculateViewMatrix();

	X = vec(1.0f, 0.0f, 0.0f);
	Y = vec(0.0f, 1.0f, 0.0f);
	Z = vec(0.0f, 0.0f, 1.0f);

	Position = vec(0.0f, 0.0f, 5.0f);
	Reference = vec(0.0f, 0.0f, 0.0f);
	
	background = Color(0.f, 0.0f, 0.f, 1.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	PLOG("Setting up the camera");
	bool ret = true;

	//App->camera->Move(vec(1.0f, 1.0f, 0.0f));
	//App->camera->LookAt(vec(0, 0, 0));
	

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

	vec newPos(0,0,0);
	float speed = 3.0f * dt;
	if(App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 8.0f * dt;

	if(App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) newPos.y += speed;
	if(App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) newPos.y -= speed;

	if(App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;


	if(App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
	if(App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

	Position += newPos;
	Reference += newPos;

	// Mouse motion ----------------
	// TODO: Requires mouse reset properly without affecting MouseMotion
	// if want to get camera rotation without pressing Right Mouse Button
	// Or move mouse around the render windows while you are pressing then be able to continously drag around

	if(App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT && true == false)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		Position -= Reference;

		if(dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;
			/*
			Quat qX = { X.x,X.y,X.z,1.0f };
			Quat qY = { Y.x,Y.y,Y.z,1.0f };
			Quat qZ = { Z.x,Z.y,Z.z,1.0f };

			qX.RotateY(DeltaX);
			qY.RotateY(DeltaX);
			qZ.RotateY(DeltaX);

			X = { qX.x, qX.y, qX.z };
			Y = { qY.x, qY.y, qY.z };
			Z = { qZ.x, qZ.y, qZ.z };
			*/
		}

		if(dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;
			/*
			Quat qY = { Y.x,Y.y,Y.z,1.0f };
			Quat qZ = { Z.x,Z.y,Z.z,1.0f };

			qY.RotateY(DeltaY);
			qZ.RotateY(DeltaY);

			Y = { qY.x, qY.y, qY.z };
			Z = { qZ.x, qZ.y, qZ.z };
			*/
			if(Y.y < 0.0f)
			{
				Z = vec(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = Z.Cross(X);
			}
		}

		Position = Reference + Z * Position.Length();//length(btQuaternion(Position.x, Position.y,));
	}
	

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec &Position, const vec &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = (Position - Reference);
	Z.Normalized();

	X = vec(0.0f, 1.0f, 0.0f).Cross(Z);
	X.Normalized();

	Y = Z.Cross(X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec &Spot)
{
	Reference = Spot;

	Z = (Position - Reference);
	Z.Normalized();

	X = vec(0.0f, 1.0f, 0.0f).Cross(Z);
	vec test = X;
	X.Normalized();

	if (X.x == test.x && X.y == test.y && X.z == test.z)
		PLOG("FUCK OFF\n")

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
float* ModuleCamera3D::GetViewMatrix()
{
	//ViewMatrix.Transpose();
	return (float*)ViewMatrix.v;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{

	/*ViewMatrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f,	-dot(btQuaternion(X.x,X.y,X.z,X.Length()), btQuaternion(Position.x, Position.y, Position.z, Position.Length())), 
																							-dot(btQuaternion(Y.x,Y.y,Y.z,Y.Length()), btQuaternion(Position.x, Position.y, Position.z, Position.Length())), 
																							-dot(btQuaternion(Z.x,Z.y,Z.z,Z.Length()), btQuaternion(Position.x, Position.y, Position.z, Position.Length())), 1.0f);
	*/
	/*ViewMatrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(btQuaternion(X., X.x, X.y, X.z), btQuaternion(Position.Length(), Position.x, Position.y, Position.z)),
																						 -dot(btQuaternion(Y.Length(), Y.x, Y.y, Y.z), btQuaternion(Position.Length(), Position.x, Position.y, Position.z)),
																						 -dot(btQuaternion(Z.Length(), Z.x, Z.y, Z.z), btQuaternion(Position.Length(),Position.x, Position.y, Position.z)), 1.0f);*/
	//ViewMatrix = float4x4({ X.x, Y.x, Z.x, 1.0f }, { X.y, Y.y, Z.y, 1.0f }, { X.z, Y.z, Z.z, 1.0f }, { -X.Dot(Position), -Y.Dot(Position), -Z.Dot(Position), 1.0f });

	//ViewMatrix = float4x4({ X.x, X.y, X.z, 1.0f }, { Y.x, Y.y, Y.z, 1.0f }, { Z.x, Z.y, Z.z, 1.0f }, { -X.Dot(Position), -Y.Dot(Position), -Z.Dot(Position), 1.0f });

	// ViewMatrix = float4x4({ {X.x, Y.x, Z.x, 0.0f },{  X.y, Y.y, Z.y, 0.0f },{  X.z, Y.z, Z.z, 0.0f },{  -X.Dot(Position), -Y.Dot(Position), -Z.Dot(Position), 1.0f} });
	ViewMatrix = float4x4(X.x, Y.x, Z.x, 0.0f ,  X.y, Y.y, Z.y, 0.0f ,  X.z, Y.z, Z.z, 0.0f ,  -X.Dot(Position), -Y.Dot(Position), -Z.Dot(Position), 1.0f);

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