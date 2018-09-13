#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(3, 1, 5);
	car.chassis_offset.Set(0, 0.75f, 0);
	car.mass = 400.0f;
	car.suspensionStiffness = 30.0f;
	car.suspensionCompression = 1.0f;
	car.suspensionDamping = 2.0f;
	car.maxSuspensionTravelCm = 400.0f;
	car.frictionSlip = 10.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.5f;
	float wheel_width = 0.3f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;
	
	vec3 direction(0,-1,0);
	vec3 axis(-1,0,0);
	
	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(0, 3, 10);
	
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

//
//vec3 ModulePlayer::bttovec3(btVector3 vector)
//{
//	vec3 ret;
//
//	ret.x = vector.x();
//	ret.y = vector.y();
//	ret.z = vector.z();
//
//	return ret;
//}


// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	turn = acceleration = brake = 0.0f;

	vec3 airturn = { 0,0,0 };

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		acceleration = MAX_ACCELERATION;

		airturn = { 0, 0, 2.5f };
	}

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if(turn < TURN_DEGREES)
			turn +=  TURN_DEGREES;

		airturn = { -1.5f,0,0 };
	}

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if(turn > -TURN_DEGREES)
			turn -= TURN_DEGREES;

		
		airturn = { 1.5f, 0, 0 };
	}

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		acceleration = -MAX_ACCELERATION/2;

		
		airturn = { 0,0,-2.5f };
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
	{
		brake = BRAKE_POWER;
	}

	//AirTurn(airturn);
	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	vehicle->Render();

	vec3 cam_org;
	vec3 cam_dest;

	btTransform transform = App->player->vehicle->vehicle->getChassisWorldTransform();
	btVector3 car_pos = transform.getOrigin();
	btVector3 car_dir = transform.getBasis().getColumn(2);

	float angle = atan2(car_dir.z(), car_dir.x())+ 3.1415926535897932384626433832795028841971693993751058209749445923078164;
	
	cam_org.x = 10 * cos(angle) + car_pos.x();
	cam_org.z = 10 * sin(angle) + car_pos.z();
	cam_org.y = 7 + car_pos.y();

	cam_dest.x = car_pos.x();
	cam_dest.y = car_pos.y();
	cam_dest.z = car_pos.z();

	App->camera->Look(cam_org, cam_dest);

	char title[80];
	sprintf_s(title, "%.1f Km/h", vehicle->GetKmh());
	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}

void ModulePlayer::AirTurn(vec3 & turn)
{
	if (turn.x != 0 || turn.z != 0) {

		int dir_axis = vehicle->vehicle->getForwardAxis();
		btVector3 n_dir_axis = vehicle->vehicle->getForwardVector();
		
		vec3 test = { n_dir_axis.getX(), n_dir_axis.getY(), n_dir_axis.getZ() };
		if (test.x >= test.y && test.x >= test.z) {
			test.y /= test.x;
			test.z /= test.x;
			test.x = 1;
		}
		else if (test.y >= test.x && test.y >= test.z) {
			test.x /= test.y;
			test.z /= test.y;
			test.y = 1;
		}
		else if (test.z >= test.y && test.z >= test.x) {
			test.y /= test.z;
			test.x /= test.z;
			test.z = 1;
		}

		turn = RotatedTurn(turn, test);

		
		vehicle->PushDyn(-10, turn);
	
	}
	
}

vec3 ModulePlayer::RotatedTurn(vec3& turn, vec3& axis)
{
	vec3 ret = { 0,0,0 };
	// x= yz y = xz z = yx

	vec3 Euler = { (float)acos((turn.y*axis.y + turn.z*axis.z) / ((sqrt(turn.y*turn.y + turn.z*turn.z)*(axis.y*axis.y + axis.z*axis.z)))),
					(float)acos((turn.x*axis.x + turn.z*axis.z) / ((sqrt(turn.x*turn.x + turn.z*turn.z)*(axis.x*axis.x + axis.z*axis.z)))),
					(float)acos((turn.y*axis.y + turn.x*axis.x) / ((sqrt(turn.y*turn.y + turn.x*turn.x)*(axis.y*axis.y + axis.x*axis.x)))) };

	vec3 turnx = {turn.x, 0, 0};
	vec3 turny = { 0,turn.y,0 };
	vec3 turnz = { 0,0,turn.z };

	mat4x4 rot_x = rotate(Euler.x, turnx);
	mat4x4 rot_y = rotate(Euler.y, turny);
	mat4x4 rot_z = rotate(Euler.z, turnz);

	ret = { (turnx.x + turny.x + turnz.x),(turnx.y + turny.y + turnz.y),(turnx.z + turny.z + turnz.z) };

	return ret;
}