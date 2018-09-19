#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

#include "MGL/MathGeoLib.h"
#include "MGL/MathGeoLibFwd.h"
#include "Color.h"

#include "Bullet/include/btBulletDynamicsCommon.h"
#include "Bullet\include\LinearMath\btMatrixX.h"

#define IdentityMatrix4x4 btMatrixX<float>({{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}})

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder
};

class Primitive
{
public:

	Primitive();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const btVector3 &u);
	void			Scale(float x, float y, float z);
	PrimitiveTypes	GetType() const;

public:
	
	Color color;
	btMatrixX<float> transform = btMatrixX<float>(4, 4);
	bool axis,wire;

protected:
	PrimitiveTypes type;
};

// ============================================
class PCube : public Primitive
{
public :
	PCube();
	PCube(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
public:
	btVector3 size;
};

// ============================================
class PSphere : public Primitive
{
public:
	PSphere();
	PSphere(float radius);
	void InnerRender() const;
public:
	float radius;
};

// ============================================
class PCylinder : public Primitive
{
public:
	PCylinder();
	PCylinder(float radius, float height);
	void InnerRender() const;
public:
	float radius;
	float height;
};

// ============================================
class PLine : public Primitive
{
public:
	PLine();
	PLine(float x, float y, float z);
	void InnerRender() const;
public:
	btVector3 origin;
	btVector3 destination;
};

// ============================================
class PPlane : public Primitive
{
public:
	PPlane();
	PPlane(float x, float y, float z, float d);
	void InnerRender() const;
public:
	btVector3 normal;
	float constant;
};

#endif // !_PRIMITIVE_H_