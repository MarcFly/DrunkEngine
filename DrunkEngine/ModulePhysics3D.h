#ifndef _MODULE_PHYSICS_3D_
#define _MODULE_PHYSICS_3D_

#include "Module.h"
#include "Globals.h"
#include "p2List.h"
#include "Primitive.h"

/*#include "MGL\MathGeoLib.h"
#include "MGL\MathGeoLibFwd.h"*/

#include "Bullet/include/btBulletDynamicsCommon.h"

// Recommended scale is 1.0f == 1 meter, no less than 0.2 objects
#define GRAVITY btVector3(0.0f, -10.0f, 0.0f) 

class DebugDrawer;
struct PhysBody3D;

class ModulePhysics3D : public Module
{
public:
	ModulePhysics3D(Application* app, bool start_enabled = true);
	~ModulePhysics3D();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	PhysBody3D* AddBody(const PSphere& sphere, float mass = 1.0f); // This creates a mathematical sphere and technically a renderable sphere (passed as the transform of a polyhedron)
	PhysBody3D* AddBody(const PCube& cube, float mass = 1.0f);
	PhysBody3D* AddBody(const PCylinder& cylinder, float mass = 1.0f);

	// Create the MathObjs
	void AddMBody(const Sphere& sphere) {
		Sphere* add = new Sphere(sphere); spheres.add(add);
	}

	//void AddConstraintP2P(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec& anchorA, const vec& anchorB);
	//void AddConstraintHinge(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec& anchorA, const vec& anchorB, const vec& axisS, const vec& axisB, bool disable_collision = false);

private:

	bool debug;

	btDefaultCollisionConfiguration*	collision_conf;
	btCollisionDispatcher*				dispatcher;
	btBroadphaseInterface*				broad_phase;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld*			world;
	btDefaultVehicleRaycaster*			vehicle_raycaster;
	DebugDrawer*						debug_draw;

	p2List<btCollisionShape*> shapes;
	p2List<PhysBody3D*> bodies;
	p2List<btDefaultMotionState*> motions;
	p2List<btTypedConstraint*> constraints;

	// Math Objects list, I don't know how to make a "MathObj" list
	p2List<Sphere*> spheres;
	p2List<Capsule*> capsules;
	p2List<AABB*> cubes;
	p2List<OBB*> obbs;
	p2List<Plane*> planes;
	p2List<Ray*> rays;
	p2List<Triangle*> tris;

	p2List<Cylinder*> cylinders;
	p2List<Frustum*> frustums;
	p2List<LineSegment*> segments;
	p2List<btConvexHullShape*> convex_hull_shapes;
	p2List<TriangleMesh*> tri_meshes;
};

class DebugDrawer : public btIDebugDraw
{
public:
	DebugDrawer() : line(0,0,0)
	{}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
	void reportErrorWarning(const char* warningString);
	void draw3dText(const btVector3& location, const char* textString);
	void setDebugMode(int debugMode);
	int	 getDebugMode() const;

	DebugDrawModes mode;
	PLine line;
	Primitive point;
};

#endif