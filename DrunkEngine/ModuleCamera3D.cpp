#include "Globals.h"
#include "Application.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
	CalculateViewMatrix();

	X = btVector3(1.0f, 0.0f, 0.0f);
	Y = btVector3(0.0f, 1.0f, 0.0f);
	Z = btVector3(0.0f, 0.0f, 1.0f);

	Position = btVector3(0.0f, 0.0f, 5.0f);
	Reference = btVector3(0.0f, 0.0f, 0.0f);
	
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	PLOG("Setting up the camera");
	bool ret = true;
	
	App->camera->Move(btVector3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(btVector3(0, 0, 0));
	
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
	
	btVector3 newPos(0,0,0);
	float speed = 3.0f * dt;
	if(App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 8.0f * dt;

	if(App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) newPos.setY(newPos.getY() + speed);
	if(App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) newPos.setY(newPos.getY() + speed);

	if(App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos.setZ(newPos.getZ() + speed);
	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos.setZ(newPos.getZ() + speed);


	if(App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos.setX(newPos.getX() + speed);
	if(App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos.setX(newPos.getX() + speed);

	Position += newPos;
	Reference += newPos;

	// Mouse motion ----------------
	// TODO: Requires mouse reset properly without affecting MouseMotion
	// if want to get camera rotation without pressing Right Mouse Button
	// Or move mouse around the render windows while you are pressing then be able to continously drag around

	if(App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		Position -= Reference;

		if(dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			btQuaternion qX = { X.getX(),X.getY(),X.getZ(),1.0f };
			btQuaternion qY = { Y.getX(),Y.getY(),Y.getZ(),1.0f };
			btQuaternion qZ = { Z.getX(),Z.getY(),Z.getZ(),1.0f };

			qX.setRotation({ 0,1,0 }, DeltaX);
			qY.setRotation({ 0,1,0 }, DeltaX);
			qZ.setRotation({ 0,0,1 }, DeltaX);
			//X = rotate(X, DeltaX, vec(0.0f, 1.0f, 0.0f));
			//Y = rotate(Y, DeltaX, vec(0.0f, 1.0f, 0.0f));
			//Z = rotate(Z, DeltaX, vec(0.0f, 1.0f, 0.0f));

			X = { qX.getX(), qX.getY(), qX.getZ() };
			Y = { qY.getX(), qY.getY(), qY.getZ() };
			Z = { qZ.getX(), qZ.getY(), qZ.getZ() };
		}

		if(dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			btQuaternion qY = { Y.getX(),Y.getY(),Y.getZ(),1.0f };
			btQuaternion qZ = { Z.getX(),Z.getY(),Z.getZ(),1.0f };

			qY.setRotation({ 0,1,0 },DeltaY);
			qZ.setRotation({ 0,0,1 }, DeltaY);
			//Y = rotate(Y, DeltaY, X);
			//Z = rotate(Z, DeltaY, X)/

			Y = { qY.getX(), qY.getY(), qY.getZ() };
			Z = { qZ.getX(), qZ.getY(), qZ.getZ() };

			if(Y.getY() < 0.0f)
			{
				Z = btVector3(0.0f, Z.getY() > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = Z.cross(X);
			}
		}

		length(btQuaternion(Position.getX(), Position.getY(), Position.getZ()));
	}

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const btVector3 &Position, const btVector3 &Reference, bool RotateAroundReference)
{
	
	this->Position = Position;
	this->Reference = Reference;

	Z = (Position - Reference);
	Z.normalize();

	X = btVector3(0.0f, 1.0f, 0.0f).cross(Z);
	X.normalize();

	Y = Z.cross(X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
	
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const btVector3 &Spot)
{
	Reference = Spot;

	Z = (Position - Reference);
	Z.normalize();

	X = btVector3(0.0f, 1.0f, 0.0f).cross(Z);
	X.normalize();

	Y = Z.cross(X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const btVector3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &(float)ViewMatrix(0, 0);
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	/*ViewMatrix.setElem = (X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f,	-dot(btQuaternion(X.x,X.y,X.z,X.Length()), btQuaternion(Position.x, Position.y, Position.z, Position.Length())), 
																							-dot(btQuaternion(Y.x,Y.y,Y.z,Y.Length()), btQuaternion(Position.x, Position.y, Position.z, Position.Length())), 
																							-dot(btQuaternion(Z.x,Z.y,Z.z,Z.Length()), btQuaternion(Position.x, Position.y, Position.z, Position.Length())), 1.0f);

	*/
	ViewMatrix.setIdentity();
	ViewMatrixInverse = ViewMatrix.transpose();
	
}