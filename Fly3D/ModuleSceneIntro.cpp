#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	Sensor* s1 = new Sensor;
	s1->shape = new Cube;
	s1->shape->size = vec3(12, 3, 1);
	s1->shape->SetPos(0, 2.501f, 20);

	s1->body = App->physics->AddBody(*s1->shape, 0.0f);
	s1->body->SetAsSensor(true);
	s1->body->collision_listeners.add(this);

	s1->id = 1;

	sensors.add(s1);


	Sensor* s2 = new Sensor;
	s2->shape = new Cube;
	s2->shape->size = vec3(23, 3, 1);
	s2->shape->SetPos(10, 12.501f, 87);
	s2->shape->SetRotation(90, {0,1,0});

	s2->body = App->physics->AddBody(*s2->shape, 0.0f);
	s2->body->SetAsSensor(true);
	s2->body->collision_listeners.add(this);

	s2->id = 2;

	sensors.add(s2);
	
	Sensor* s3 = new Sensor;
	s3->shape = new Cube;
	s3->shape->size = vec3(16, 3, 1);
	s3->shape->SetPos(0, 12.501f, 5);
	s3->shape->SetRotation(90, { 0,1,0 });

	s3->body = App->physics->AddBody(*s3->shape, 0.0f);
	s3->body->SetAsSensor(true);
	s3->body->collision_listeners.add(this);

	s3->id = 3;

	sensors.add(s3);

	Sensor* s4 = new Sensor;
	s4->shape = new Cube;
	s4->shape->size = vec3(20, 3, 1);
	s4->shape->SetPos(-75, 2.501f, 50);

	s4->body = App->physics->AddBody(*s4->shape, 0.0f);
	s4->body->SetAsSensor(true);
	s4->body->collision_listeners.add(this);

	s4->id = 4;

	sensors.add(s4);

	Sensor* s5 = new Sensor;
	s5->shape = new Cube;
	s5->shape->size = vec3(18, 3, 1);
	s5->shape->SetPos(-23, 2.501f, 78);
	s5->shape->SetRotation(120, { 0,1,0 });

	s5->body = App->physics->AddBody(*s5->shape, 0.0f);
	s5->body->SetAsSensor(true);
	s5->body->collision_listeners.add(this);

	s5->id = 5;

	sensors.add(s5);

	Sensor* s6 = new Sensor;
	s6->shape = new Cube;
	s6->shape->size = vec3(18, 3, 1);
	s6->shape->SetPos(21, 2.501f, -50);

	s6->body = App->physics->AddBody(*s6->shape, 0.0f);
	s6->body->SetAsSensor(true);
	s6->body->collision_listeners.add(this);

	s6->id = 6;

	sensors.add(s6);
	/*p.normal = { 0,1,0 };
	p.constant = 0;*/
	pl = new Cube;
	pl->size = { 1000, 1, 1000 };
	pl->SetPos(10, 0, 0);
	

	plane_sensor = App->physics->AddBody(*pl, 0);
	plane_sensor->SetAsSensor(true);
	plane_sensor->collision_listeners.add(this);

	//Create Map

	//principal circuit
	{//1
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(14, 1, 50);
		shape->SetPos(-1, shape->size.y / 2, 0);

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}
	{//2
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(15, 1, 20);
		shape->SetPos(-6.7, shape->size.y / 2, 19.5 + shape->size.z/2);
		shape->SetRotation(-30, {0,1,0});

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}
	{//3
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(15, 1, 30);
		shape->SetPos(-10, shape->size.y / 2 + 5, 48.0f);
		shape->SetRotation(-20, {1,0,0});

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//4
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(15, 1, 15);
		shape->SetPos(-10, shape->size.y / 2 + 10.2, 69.2f);

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}


	{//5
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(20, 1, 30);
		shape->SetPos(0, shape->size.y / 2 + 10.2, 79.8f);

		//float inc_y = sin(45 * 3.14/180);
		//float inc_z = sin(60 * 3.14 / 180);

		shape->SetRotation(45, {0,1,0 });
		//shape->SetRotation( 90, { 0,0,inc_z });

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//6
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(15, 1, 30);
		shape->SetPos(19, shape->size.y / 2 + 10.2, 91.0f);

		//float inc_y = sin(45 * 3.14/180);
		//float inc_z = sin(60 * 3.14 / 180);

		shape->SetRotation(90, { 0,1,0 });
		//shape->SetRotation( 90, { 0,0,inc_z });

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//7
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(17, 1, 30);
		shape->SetPos(36, shape->size.y / 2 + 10.2, 82.0f);

		//float inc_y = sin(45 * 3.14/180);
		//float inc_z = sin(60 * 3.14 / 180);

		shape->SetRotation(135, { 0,1,0 });
		//shape->SetRotation( 40, { 0,0,1 });

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//8
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(15, 1, 60);
		shape->SetPos(45, shape->size.y / 2 + 10.2, 50.2f);

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//9
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(15, 1, 30);
		shape->SetPos(40, shape->size.y / 2 + 10.2, 15.0f);

		shape->SetRotation(30, { 0,1,0 });

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//10
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(35, 1, 15);
		shape->SetPos(20, shape->size.y / 2 + 10.2, 5.5f);

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//11
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(20, 1, 15);
		shape->SetPos(-7, shape->size.y / 2 + 8.5f, 5.5f);
	
		shape->SetRotation(10, {0,0,1});

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//12
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(25, 1, 15);
		shape->SetPos(-29, shape->size.y / 2 + 3.5f, 5.5f);

		shape->SetRotation(15, { 0,0,1 });

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//13
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(25, 1, 15);
		shape->SetPos(-50, shape->size.y / 2, 5.5f);

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//14
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(20, 1, /**/18);
		shape->SetPos(-65, shape->size.y / 2, 10);
		shape->SetRotation(45, { 0,1,0 });

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//15
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(20, 1, /**/18);
		shape->SetPos(-73, shape->size.y / 2, 22);
		shape->SetRotation(75, { 0,1,0 });

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//16
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(/**/18, 1, 35);
		shape->SetPos(-75, shape->size.y / 2, 47.5);

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//17
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(/**/18, 1, 20);
		shape->SetPos(-71, shape->size.y / 2, 70);
		shape->SetRotation(30, { 0,1,0 });

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//18
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(/**/18, 1, 20);
		shape->SetPos(-62, shape->size.y / 2, 80);
		shape->SetRotation(60, { 0,1,0 });

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//19
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(30, 1, 18/**/);
		shape->SetPos(-48, shape->size.y / 2, 85);

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//20
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(18/**/, 1, 35);
		shape->SetPos(-23, shape->size.y / 2, 78);
		shape->SetRotation(120, { 0,1,0 });

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//21
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(18/**/, 1, 70);
		shape->SetPos(0, shape->size.y / 2, 58);
		shape->SetRotation(140, { 0,1,0 });

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//22
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(18/**/, 1, 35);
		shape->SetPos(22, shape->size.y / 2, 20);

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//23
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(18/**/, 1, 10);
		shape->SetPos(22, shape->size.y / 2 + 0.5, -2);
		shape->SetRotation(6, { 1,0,0 });

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	//{//24
	//	CircuitPart* part = new CircuitPart;
	//	Cube* shape = new Cube;
	//	shape->size = vec3(18/**/, 1, 10);
	//	shape->SetPos(22, shape->size.y / 2 + 1, -40);
	//	shape->SetRotation(-10, { 1,0,0 });
	//	part->body = App->physics->AddBody(*shape, 0);
	//	part->shape = shape;
	//	circuit.add(part);
	//}

	{//24
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(18/**/, 1, 60);
		shape->SetPos(22, shape->size.y / 2, -55);

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//25
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(18/**/, 1, 25);
		shape->SetPos(15, shape->size.y / 2, -89);
		shape->SetRotation(45, { 0,1,0 });

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//26
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(18/**/, 1, 25);
		shape->SetPos(8, shape->size.y / 2, -89);
		shape->SetRotation(135, { 0,1,0 });

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//27
		CircuitPart* part = new CircuitPart;
		Cube* shape = new Cube;

		shape->size = vec3(18/**/, 1, 65);
		shape->SetPos(0, shape->size.y / 2, -57);

		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

//HAZARDS
	{//1
		CircuitPart* part = new CircuitPart;
		Sphere* shape = new Sphere;

		shape->radius = 3;
		//shape->height = 3;
			shape->SetPos(0, -1, 57);
			shape->SetRotation(90, { 0,0,1 });


		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//2
		CircuitPart* part = new CircuitPart;
		Sphere* shape = new Sphere;

		shape->radius = 3;
		//shape->height = 3;
		shape->SetPos(1, -1, 45);
		shape->SetRotation(90, { 0,0,1 });


		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//3
		CircuitPart* part = new CircuitPart;
		Sphere* shape = new Sphere;

		shape->radius = 3;
		//shape->height = 3;
		shape->SetPos(10, -1, 35);
		shape->SetRotation(90, { 0,0,1 });


		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//4
		CircuitPart* part = new CircuitPart;
		Sphere* shape = new Sphere;

		shape->radius = 3;
		//shape->height = 3;
		shape->SetPos(-5, -1, 70);
		shape->SetRotation(90, { 0,0,1 });


		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//5
		CircuitPart* part = new CircuitPart;
		Sphere* shape = new Sphere;

		shape->radius = 3;
		//shape->height = 3;
		shape->SetPos(5, -1, 60);
		shape->SetRotation(90, { 0,0,1 });


		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//6
		CircuitPart* part = new CircuitPart;
		Sphere* shape = new Sphere;

		shape->radius = 3;
		//shape->height = 3;
		shape->SetPos(15, -1, 50);
		shape->SetRotation(90, { 0,0,1 });


		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}

	{//7
		CircuitPart* part = new CircuitPart;
		Sphere* shape = new Sphere;

		shape->radius = 3;
		//shape->height = 3;
		shape->SetPos(10, -1, 45);
		shape->SetRotation(90, { 0,0,1 });


		part->body = App->physics->AddBody(*shape, 0);
		part->shape = shape;
		circuit.add(part);
	}
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	


	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	pl->Render();

	p2List_item<Sensor*>* item = sensors.getFirst();
	while (item != NULL) {
		item->data->shape->Render();
		item = item->next;
	}

	p2List_item<CircuitPart*>*  item_2 = circuit.getFirst();

	while (item_2 != NULL) {
		
		item_2->data->shape->Render();

		item_2 = item_2->next;
	}

	
	

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if (body1->IsSensor()) {
		if (body1 == plane_sensor) {
			if(body2->body->getCenterOfMassPosition() == App->player->vehicle->body->getCenterOfMassPosition())
				set_to_cp = true;
		
		}
		else {
			p2List_item<Sensor*> *item = sensors.getFirst();
			while (item != NULL) {
				if (body1 == item->data->body) {
					last_cp = item->data->id;
					break;
				}
				item = item->next;
			}
		}

		
	}
	else if (body2->IsSensor()) {
		if (body2 == plane_sensor) {
			if (body1->body->getCenterOfMassTransform() == App->player->vehicle->body->getCenterOfMassTransform());
				set_to_cp = true;
		
		}
		else {
			p2List_item<Sensor*> *item = sensors.getFirst();
			while (item != NULL) {
				if (body2 == item->data->body) {
					last_cp = item->data->id;
					break;
				}
				item = item->next;
			}
		}
	}

}

void ModuleSceneIntro::SetToCP(PhysVehicle3D* v){
	p2List_item<Sensor*>*item = sensors.getFirst();
	while (item != NULL) {
		if (item->data->id == last_cp)
			break;
		item = item->next;
	}

	v->body->setCenterOfMassTransform(item->data->body->body->getCenterOfMassTransform());

}

