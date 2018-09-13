#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "glmath.h"

struct PhysVehicle3D;

#define MAX_ACCELERATION 1000.0f
#define TURN_DEGREES 20.0f * DEGTORAD
#define BRAKE_POWER 1000.0f

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:

	PhysVehicle3D* vehicle;
	float turn;
	float acceleration;
	float brake;

public:
	//Own Code, new Mechanic - Aerial Control

	void AirTurn(vec3& turn);

	vec3 RotatedTurn(vec3& turn, vec3& axis);

	//vec3 bttovec3(btVector3 vector);


public:
	int laps;

};