#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include <list>
#include "Module.h"

class btRigidBody;
class Module;

// =================================================
struct PhysBody3D
{
	friend class ModulePhysics3D;
public:
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);
	void SetAsSensor(bool is_sensor);
	bool IsSensor() const;

private:
	
	bool is_sensor = false;

public:
	std::list<Module*> collision_listeners;
	btRigidBody* body = nullptr;

public:
	//Own Code, new Mechanincs
	void PushDyn(float magnitude, vec& test);
};

#endif // __PhysBody3D_H__