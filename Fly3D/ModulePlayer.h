#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "glmath.h"

struct PhysVehicle3D;

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:


};