#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

#include "Color.h"
#include "Module.h"

//using namespace math;

#define IdentityMatrix4x4 float4x4({{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}})

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
	void			SetRotation(float angle, const vec &u);
	void			Scale(float x, float y, float z);
	PrimitiveTypes	GetType() const;

	virtual bool Intersects(Primitive* mbody2) { return false;};

public:
	
	Color color;
	float4x4 transform;
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
	PCube(float size_cube);
	void InnerRender() const;
	bool Intersects(Primitive* mbody2);

public:
	vec size;
	AABB MathBody;
};

// ============================================
class PSphere : public Primitive
{
public:
	PSphere();
	PSphere(float radius);
	void InnerRender() const;
	bool Intersects(Primitive* mbody2);

public:
	float radius;
	Sphere MathBody;
};

// ============================================
class PCylinder : public Primitive
{
public:
	PCylinder();
	PCylinder(float radius, float height);
	void InnerRender() const;
	bool Intersects(Primitive* mbody2);

public:
	float radius;
	float height;
	Capsule MathBody;
};

// ============================================
class PLine : public Primitive
{
public:
	PLine();
	PLine(float x, float y, float z);
	void InnerRender() const;
	bool Intersects(Primitive* mbody2);

public:
	vec origin;
	vec destination;
	Line MathBody;
};

// ============================================
class PPlane : public Primitive
{
public:
	PPlane();
	PPlane(float x, float y, float z, float d);
	void InnerRender() const;
	bool Intersects(Primitive* mbody2);

public:
	vec normal;
	float constant;
	Plane MathBody;
};

#endif // !_PRIMITIVE_H_