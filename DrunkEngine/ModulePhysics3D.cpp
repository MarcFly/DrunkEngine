#include "Application.h"
#include "ModulePhysics3D.h"
#include "PhysBody3D.h"


#ifdef _DEBUG
	#pragma comment (lib, "Bullet/libx86/BulletDynamics_debug.lib")
	#pragma comment (lib, "Bullet/libx86/BulletCollision_debug.lib")
	#pragma comment (lib, "Bullet/libx86/LinearMath_debug.lib")
#else
	#pragma comment (lib, "Bullet/libx86/BulletDynamics.lib")
	#pragma comment (lib, "Bullet/libx86/BulletCollision.lib")
	#pragma comment (lib, "Bullet/libx86/LinearMath.lib")
#endif

ModulePhysics3D::ModulePhysics3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;

	collision_conf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_conf);
	broad_phase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	debug_draw = new DebugDrawer();
}

// Destructor
ModulePhysics3D::~ModulePhysics3D()
{
	delete debug_draw;
	delete solver;
	delete broad_phase;
	delete dispatcher;
	delete collision_conf;
}

// Render not available yet----------------------------------
bool ModulePhysics3D::Init()
{
	PLOG("Creating 3D Physics simulation");
	bool ret = true;

	return ret;
}

// ---------------------------------------------------------
bool ModulePhysics3D::Start()
{
	PLOG("Creating Physics environment");

	world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);
	world->setDebugDrawer(debug_draw);
	world->setGravity(GRAVITY);

	// Debug Plane for axis control
	{
		btCollisionShape* colShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);

		btDefaultMotionState* myMotionState = new btDefaultMotionState();
		btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, myMotionState, colShape);

		btRigidBody* body = new btRigidBody(rbInfo);
		world->addRigidBody(body);
	}

	return true;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PreUpdate(float dt)
{
	// Not used for now
	
	world->stepSimulation(dt, 15);
	
	// Bullet Collision detection
	int numManifolds = world->getDispatcher()->getNumManifolds();
	for(int i = 0; i<numManifolds; i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

		int numContacts = contactManifold->getNumContacts();
		if(numContacts > 0)
		{
			PhysBody3D* pbodyA = (PhysBody3D*)obA->getUserPointer();
			PhysBody3D* pbodyB = (PhysBody3D*)obB->getUserPointer();

			if(pbodyA && pbodyB)
			{
				std::list<Module*>::iterator item = pbodyA->collision_listeners.begin();
				while(item != pbodyA->collision_listeners.end())
				{
					item._Ptr->_Myval->OnCollision(pbodyA, pbodyB);
					item++;
				}

				item = pbodyB->collision_listeners.begin();
				while(item != pbodyB->collision_listeners.end())
				{
					item._Ptr->_Myval->OnCollision(pbodyB, pbodyA);
					item++;
				}
			}
		}
	}

	// MathGeoLib Collision Detection
	std::list<PhysBody3D*>::iterator item_pb = bodies.begin();

	while (item_pb != bodies.end() && bodies.size() > 0)
	{
		if(item_pb._Ptr->_Myval->body != nullptr)
		{
			vec center = (vec)item_pb._Ptr->_Myval->body->getCenterOfMassPosition();
				
			item_pb._Ptr->_Myval->mbody->SetPos(center.x, center.y, center.z);
		}
			

		std::list<PhysBody3D*>::iterator item_pb_2 = ++item_pb;
		item_pb--;
		while (item_pb_2 != bodies.end())
		{
			if (item_pb._Ptr->_Myval->mbody->Intersects(item_pb_2._Ptr->_Myval->mbody))
			{
				PLOG("Detected Collision!")
			}
			item_pb_2++;
		}

		item_pb++;
	}


	// Mathbody pos update in relation to physic body, old
	/*std::list<PhysBody3D*>::iterator item = bodies.begin();
	std::list<Sphere*>::iterator item_sphere = spheres.begin();
	std::list<Capsule*>::iterator item_capsule = capsules.begin();
	std::list<AABB*>::iterator item_AABB = cubes.begin();
	std::list<OBB*>::iterator item_OBB = obbs.begin();
	std::list<Plane*>::iterator item_plane = planes.begin();
	std::list<Ray*>::iterator item_ray = rays.begin();
	std::list<Triangle*>::iterator item_tri = tris.begin();

	while (item != bodies.end() && bodies.size() > 0) {
		if (item._Ptr->_Myval->body->getCollisionShape()->getShapeType() == SPHERE_SHAPE_PROXYTYPE && item_sphere != spheres.end() && spheres.size() > 0)
		{
			item_sphere._Ptr->_Myval->Translate(((vec)item._Ptr->_Myval->body->getCenterOfMassPosition() - item_sphere._Ptr->_Myval->Centroid()));
			item_sphere++;
		}
		
		else if (item._Ptr->_Myval->body->getCollisionShape()->getShapeType() == CAPSULE_SHAPE_PROXYTYPE && item_capsule != capsules.end() && capsules.size() > 0)
		{
			item_capsule._Ptr->_Myval->Translate(((vec)item._Ptr->_Myval->body->getCenterOfMassPosition() - item_capsule._Ptr->_Myval->Center()));
			item_capsule++;
		}

		else if (item._Ptr->_Myval->body->getCollisionShape()->getShapeType() == BOX_SHAPE_PROXYTYPE && item_AABB != cubes.end() && cubes.size() > 0)
		{
			item_AABB._Ptr->_Myval->Translate(((vec)item._Ptr->_Myval->body->getCenterOfMassPosition() - item_AABB._Ptr->_Myval->Centroid()));
			item_AABB++;
		}

		else if (item._Ptr->_Myval->body->getCollisionShape()->getShapeType() == TRIANGLE_SHAPE_PROXYTYPE && item_tri != tris.end() && tris.size() > 0)
		{
			item_tri._Ptr->_Myval->Translate(((vec)item._Ptr->_Myval->body->getCenterOfMassPosition() - item_capsule._Ptr->_Myval->Center()));
			item_tri++;
		}
		// Do for all types

		else if (item_OBB != obbs.end() && obbs.size() > 0)
		{
			item_OBB._Ptr->_Myval->Translate(((vec)item._Ptr->_Myval->body->getCenterOfMassPosition() - item_OBB._Ptr->_Myval->CenterPoint()));
			item_OBB++;
		}
		item++;
		
	*/

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::Update(float dt)
{
	
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(debug == true)
	{
		world->debugDrawWorld();

	}
	
	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics3D::CleanUp()
{
	PLOG("Destroying 3D Physics simulation");

	// Remove from the world all collision bodies
	for(int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		world->removeCollisionObject(obj);
	}

	for(std::list<btTypedConstraint*>::iterator item = constraints.begin(); item != constraints.end(); item++)
	{
		world->removeConstraint(item._Ptr->_Myval);
		delete item._Ptr->_Myval;
	}
	
	constraints.clear();

	for(std::list<btDefaultMotionState*>::iterator item = motions.begin(); item != motions.end(); item++)
		delete item._Ptr->_Myval;

	motions.clear();

	for(std::list<btCollisionShape*>::iterator item = shapes.begin(); item != shapes.end(); item++)
		delete item._Ptr->_Myval;

	shapes.clear();

	for(std::list<PhysBody3D*>::iterator item = bodies.begin(); item != bodies.end(); item++)
		delete item._Ptr->_Myval;

	bodies.clear();

	delete world;
	
	return true;
}


// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const vec& center, PSphere& sphere, bool phys, float mass)
{
	btRigidBody* body = nullptr;

	if (phys == true) {
		btCollisionShape* colShape = new btSphereShape(sphere.radius);
		shapes.push_back(colShape);

		btTransform startTransform;
		startTransform.setFromOpenGLMatrix((float*)sphere.transform.v);
		startTransform.setOrigin(btVector3(center.x, center.y, center.z));

		btVector3 localInertia(0, 0, 0);
		if (mass != 0.f)
			colShape->calculateLocalInertia(mass, localInertia);

		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		motions.push_back(myMotionState);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

		body = new btRigidBody(rbInfo);

		// Add mathsphere from btbody
		AddSphereMath((vec)body->getCenterOfMassPosition(), sphere.radius);

		world->addRigidBody(body);
		
	}

	// Add Mathematical Sphere
	if(!phys)
		AddSphereMath(center, sphere.radius);

	PSphere* new_sphere = new PSphere(sphere);
	PhysBody3D* pbody = new PhysBody3D(body, new_sphere);
	bodies.push_back(pbody);

	return pbody;
}


// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const vec& center, PCube& cube,bool phys, float mass)
{
	
	btRigidBody* body = nullptr;

	if (phys) {
		btCollisionShape* colShape = new btBoxShape(btVector3(cube.size.x*0.5f, cube.size.y*0.5f, cube.size.z*0.5f));
		shapes.push_back(colShape);

		btTransform startTransform;
		startTransform.setFromOpenGLMatrix((float*)cube.transform.Transposed().v);
		startTransform.setOrigin(btVector3(center.x, center.y, center.z));

		btVector3 localInertia(0, 0, 0);
		if (mass != 0.f)
			colShape->calculateLocalInertia(mass, localInertia);

		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		motions.push_back(myMotionState);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

		body = new btRigidBody(rbInfo);
		
		// Add Mathematical Sphere
		AddAABBMath((vec)body->getCenterOfMassPosition(), cube.size.x);

		world->addRigidBody(body);
	}

	PCube* new_cube = new PCube(cube);

	if (!phys) {
		

		if (cube.size.x >= cube.size.y && cube.size.x >= cube.size.z)
		{
			new_cube->MathBody = *AddAABBMath(center, cube.size.x);
		}
		else if (cube.size.y >= cube.size.x && cube.size.y >= cube.size.z)
		{
			AddAABBMath(center, cube.size.y);
		}
		else if (cube.size.z >= cube.size.x && cube.size.z >= cube.size.y)
		{
			AddAABBMath(center, cube.size.z);
		}
	}

	
	PhysBody3D* pbody = new PhysBody3D(body, new_cube);
	bodies.push_back(pbody);

	return pbody;
}

// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(PCylinder& cylinder, bool phys, float mass)
{
	btCollisionShape* colShape = new btCylinderShapeX(btVector3(cylinder.height*0.5f, cylinder.radius, 0.0f));
	shapes.push_back(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix((float*)cylinder.transform.Transposed().v);

	btVector3 localInertia(0, 0, 0);
	if(mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.push_back(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body, &cylinder);

	world->addRigidBody(body);
	bodies.push_back(pbody);

	return pbody;
}

/*
// ---------------------------------------------------------
void ModulePhysics3D::AddConstraintP2P(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec& anchorA, const vec& anchorB)
{
	btTypedConstraint* p2p = new btPoint2PointConstraint(
		*(bodyA.body), 
		*(bodyB.body), 
		btVector3(anchorA.x, anchorA.y, anchorA.z), 
		btVector3(anchorB.x, anchorB.y, anchorB.z));
	world->addConstraint(p2p);
	constraints.push_back(p2p);
	p2p->setDbgDrawSize(2.0f);
}

void ModulePhysics3D::AddConstraintHinge(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec& anchorA, const vec& anchorB, const vec& axisA, const vec& axisB, bool disable_collision)
{
	btHingeConstraint* hinge = new btHingeConstraint(
		*(bodyA.body), 
		*(bodyB.body), 
		btVector3(anchorA.x, anchorA.y, anchorA.z),
		btVector3(anchorB.x, anchorB.y, anchorB.z),
		btVector3(axisA.x, axisA.y, axisA.z), 
		btVector3(axisB.x, axisB.y, axisB.z));

	world->addConstraint(hinge, disable_collision);
	constraints.push_back(hinge);
	hinge->setDbgDrawSize(2.0f);
}
*/

// =============================================
void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	line.origin.Set(from.getX(), from.getY(), from.getZ());
	line.destination.Set(to.getX(), to.getY(), to.getZ());
	line.color.Set(color.getX(), color.getY(), color.getZ());
	line.Render();
}

void DebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	point.transform.Translate(PointOnB.getX(), PointOnB.getY(), PointOnB.getZ());
	point.color.Set(color.getX(), color.getY(), color.getZ());
	point.Render();
}

void DebugDrawer::reportErrorWarning(const char* warningString)
{
	PLOG("Bullet warning: %s", warningString);
}

void DebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	PLOG("Bullet draw text: %s", textString);
}

void DebugDrawer::setDebugMode(int debugMode)
{
	mode = (DebugDrawModes) debugMode;
}

int	 DebugDrawer::getDebugMode() const
{
	return mode;
}
