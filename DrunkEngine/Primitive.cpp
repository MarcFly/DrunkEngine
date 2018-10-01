
#include "Globals.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Primitive.h"

//using namespace math;

// ------------------------------------------------------------
Primitive::Primitive() : transform(IdentityMatrix4x4), color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Point)
{}

// ------------------------------------------------------------
PrimitiveTypes Primitive::GetType() const
{
	return type;
}

// ------------------------------------------------------------
void Primitive::Render() const
{
	glPushMatrix();
	/*float trMatrix[16];
	for (int i = 0; i < 16; i++)
		trMatrix[i] = transform.At(i/4,i/(i/4));*/
	glMultMatrixf((float*)transform.v);

	if(axis == true)
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);

	if(wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	InnerRender();

	glPopMatrix();
}

// ------------------------------------------------------------
void Primitive::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

// ------------------------------------------------------------
void Primitive::SetPos(float x, float y, float z)
{
	transform.TransformPos(float3( x,y,z ));
}

// ------------------------------------------------------------
void Primitive::SetRotation(float angle, const vec &u)
{
	transform.SetRotatePart(u, angle);
}

// ------------------------------------------------------------
void Primitive::Scale(float x, float y, float z)
{
	transform.Scale({ x,y,z });
}

// CUBE ============================================
PCube::PCube() : Primitive(), size(1.0f, 1.0f, 1.0f)
{
	type = PrimitiveTypes::Primitive_Cube;
}

PCube::PCube(float sizeX, float sizeY, float sizeZ) : Primitive(), size(sizeX, sizeY, sizeZ)
{
	type = PrimitiveTypes::Primitive_Cube;
}
PCube::PCube(float size_cube) : Primitive(), size(size_cube, size_cube, size_cube)
{
	type = PrimitiveTypes::Primitive_Cube;
}

void PCube::InnerRender() const
{
	// Direct with random shiet i do not know what is
	/*
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	// GL_QUAD APPROACH
	glBegin(GL_QUADS);
	{
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-sx, -sy, sz);
		glVertex3f(sx, -sy, sz);
		glVertex3f(sx, sy, sz);
		glVertex3f(-sx, sy, sz);

		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(sx, -sy, -sz);
		glVertex3f(-sx, -sy, -sz);
		glVertex3f(-sx, sy, -sz);
		glVertex3f(sx, sy, -sz);

		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(sx, -sy, sz);
		glVertex3f(sx, -sy, -sz);
		glVertex3f(sx, sy, -sz);
		glVertex3f(sx, sy, sz);

		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(-sx, -sy, -sz);
		glVertex3f(-sx, -sy, sz);
		glVertex3f(-sx, sy, sz);
		glVertex3f(-sx, sy, -sz);

		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-sx, sy, sz);
		glVertex3f(sx, sy, sz);
		glVertex3f(sx, sy, -sz);
		glVertex3f(-sx, sy, -sz);

		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(-sx, -sy, -sz);
		glVertex3f(sx, -sy, -sz);
		glVertex3f(sx, -sy, sz);
		glVertex3f(-sx, -sy, sz);
	}
	*/	

	// From MGL to Cube with GL_Triangles

	glBegin(GL_TRIANGLES);
	{
		// Face 1 ADC + ABD == 032 + 013
		glVertex3f(MathBody->CornerPoint(0).x, MathBody->CornerPoint(0).y, MathBody->CornerPoint(0).z);
		glVertex3f(MathBody->CornerPoint(3).x, MathBody->CornerPoint(3).y, MathBody->CornerPoint(3).z);
		glVertex3f(MathBody->CornerPoint(2).x, MathBody->CornerPoint(2).y, MathBody->CornerPoint(2).z);
	
		glVertex3f(MathBody->CornerPoint(0).x, MathBody->CornerPoint(0).y, MathBody->CornerPoint(0).z);
		glVertex3f(MathBody->CornerPoint(1).x, MathBody->CornerPoint(1).y, MathBody->CornerPoint(1).z);
		glVertex3f(MathBody->CornerPoint(3).x, MathBody->CornerPoint(3).y, MathBody->CornerPoint(3).z);
		
		// Face 2 BFH + BHD == 157 + 173

		glVertex3f(MathBody->CornerPoint(1).x, MathBody->CornerPoint(1).y, MathBody->CornerPoint(1).z);
		glVertex3f(MathBody->CornerPoint(5).x, MathBody->CornerPoint(5).y, MathBody->CornerPoint(5).z);
		glVertex3f(MathBody->CornerPoint(7).x, MathBody->CornerPoint(7).y, MathBody->CornerPoint(7).z);

		glVertex3f(MathBody->CornerPoint(1).x, MathBody->CornerPoint(1).y, MathBody->CornerPoint(1).z);
		glVertex3f(MathBody->CornerPoint(7).x, MathBody->CornerPoint(7).y, MathBody->CornerPoint(7).z);
		glVertex3f(MathBody->CornerPoint(3).x, MathBody->CornerPoint(3).y, MathBody->CornerPoint(3).z);

		// Face 3 FEH + EGH == 547 + 467

		glVertex3f(MathBody->CornerPoint(5).x, MathBody->CornerPoint(5).y, MathBody->CornerPoint(5).z);
		glVertex3f(MathBody->CornerPoint(4).x, MathBody->CornerPoint(4).y, MathBody->CornerPoint(4).z);
		glVertex3f(MathBody->CornerPoint(7).x, MathBody->CornerPoint(7).y, MathBody->CornerPoint(7).z);

		glVertex3f(MathBody->CornerPoint(4).x, MathBody->CornerPoint(4).y, MathBody->CornerPoint(4).z);
		glVertex3f(MathBody->CornerPoint(6).x, MathBody->CornerPoint(6).y, MathBody->CornerPoint(6).z);
		glVertex3f(MathBody->CornerPoint(7).x, MathBody->CornerPoint(7).y, MathBody->CornerPoint(7).z);

		// Face 4 EAC + ECG == 402 + 426

		glVertex3f(MathBody->CornerPoint(4).x, MathBody->CornerPoint(4).y, MathBody->CornerPoint(4).z);
		glVertex3f(MathBody->CornerPoint(0).x, MathBody->CornerPoint(0).y, MathBody->CornerPoint(0).z);
		glVertex3f(MathBody->CornerPoint(2).x, MathBody->CornerPoint(2).y, MathBody->CornerPoint(2).z);

		glVertex3f(MathBody->CornerPoint(4).x, MathBody->CornerPoint(4).y, MathBody->CornerPoint(4).z);
		glVertex3f(MathBody->CornerPoint(2).x, MathBody->CornerPoint(3).y, MathBody->CornerPoint(2).z);
		glVertex3f(MathBody->CornerPoint(6).x, MathBody->CornerPoint(6).y, MathBody->CornerPoint(6).z);

		// Face 5 CDH + CHG == 237 + 276

		glVertex3f(MathBody->CornerPoint(2).x, MathBody->CornerPoint(2).y, MathBody->CornerPoint(2).z);
		glVertex3f(MathBody->CornerPoint(3).x, MathBody->CornerPoint(3).y, MathBody->CornerPoint(3).z);
		glVertex3f(MathBody->CornerPoint(7).x, MathBody->CornerPoint(7).y, MathBody->CornerPoint(7).z);

		glVertex3f(MathBody->CornerPoint(2).x, MathBody->CornerPoint(2).y, MathBody->CornerPoint(2).z);
		glVertex3f(MathBody->CornerPoint(7).x, MathBody->CornerPoint(7).y, MathBody->CornerPoint(7).z);
		glVertex3f(MathBody->CornerPoint(6).x, MathBody->CornerPoint(6).y, MathBody->CornerPoint(6).z);

		// Face 6 AEF + AFB == 045 + 051

		glVertex3f(MathBody->CornerPoint(0).x, MathBody->CornerPoint(0).y, MathBody->CornerPoint(0).z);
		glVertex3f(MathBody->CornerPoint(4).x, MathBody->CornerPoint(4).y, MathBody->CornerPoint(4).z);
		glVertex3f(MathBody->CornerPoint(5).x, MathBody->CornerPoint(5).y, MathBody->CornerPoint(5).z);

		glVertex3f(MathBody->CornerPoint(0).x, MathBody->CornerPoint(0).y, MathBody->CornerPoint(0).z);
		glVertex3f(MathBody->CornerPoint(5).x, MathBody->CornerPoint(5).y, MathBody->CornerPoint(5).z);
		glVertex3f(MathBody->CornerPoint(1).x, MathBody->CornerPoint(1).y, MathBody->CornerPoint(1).z);
	}

	// 

	glEnd();
}

// SPHERE ============================================
PSphere::PSphere() : Primitive(), radius(1.0f)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

PSphere::PSphere(float radius) : Primitive(), radius(radius)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

void PSphere::InnerRender() const
{
	//TODO
	//glutSolidSphere(radius, 25, 25);

}


// CYLINDER ============================================
PCylinder::PCylinder() : Primitive(), radius(1.0f), height(1.0f)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

PCylinder::PCylinder(float radius, float height) : Primitive(), radius(radius), height(height)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

void PCylinder::InnerRender() const
{
	int n = 30;

	// Cylinder Bottom
	glBegin(GL_POLYGON);
	
	for(int i = 360; i >= 0; i -= (360 / n))
	{
		float a = i * pi / 180; // degrees to radians
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder Top
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	for(int i = 0; i <= 360; i += (360 / n))
	{
		float a = i * pi / 180; // degrees to radians
		glVertex3f(height * 0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder "Cover"
	glBegin(GL_QUAD_STRIP);
	for(int i = 0; i < 480; i += (360 / n))
	{
		float a = i * pi / 180; // degrees to radians

		glVertex3f(height*0.5f,  radius * cos(a), radius * sin(a) );
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a) );
	}
	glEnd();
}

// LINE ==================================================
PLine::PLine() : Primitive(), origin(0, 0, 0), destination(1, 1, 1)
{
	type = PrimitiveTypes::Primitive_Line;
}

PLine::PLine(float x, float y, float z) : Primitive(), origin(0, 0, 0), destination(x, y, z)
{
	type = PrimitiveTypes::Primitive_Line;
}

void PLine::InnerRender() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();

	glLineWidth(1.0f);
}

// PLANE ==================================================
PPlane::PPlane() : Primitive(), normal(0, 1, 0), constant(1)
{
	type = PrimitiveTypes::Primitive_Plane;
}

PPlane::PPlane(float x, float y, float z, float d) : Primitive(), normal(x, y, z), constant(d)
{
	type = PrimitiveTypes::Primitive_Plane;
}

void PPlane::InnerRender() const
{
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	float d = 200.0f;

	for(float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}

bool PLine::Intersects(Primitive* mbody2)
{
	switch (mbody2->GetType()) {
		case 2: return ((PPlane*)mbody2)->MathBody->Intersects(*MathBody);
		case 3: return ((PCube*)mbody2)->MathBody->Intersects(*MathBody);
		case 4: return ((PSphere*)mbody2)->MathBody->Intersects(*MathBody);
		case 5: return ((PCylinder*)mbody2)->MathBody->Intersects(*MathBody);
		default: return false;
	}

	return false;
}

bool PPlane::Intersects(Primitive* mbody2)
{
	switch (mbody2->GetType()) {
	case 2: return ((PPlane*)mbody2)->MathBody->Intersects(*MathBody);
	case 3: return ((PCube*)mbody2)->MathBody->Intersects(*MathBody);
	case 4: return ((PSphere*)mbody2)->MathBody->Intersects(*MathBody);
	case 5: return ((PCylinder*)mbody2)->MathBody->Intersects(*MathBody);
	default: return false;
	}

	return false;
}

bool PCube::Intersects(Primitive* mbody2)
{
	switch (mbody2->GetType()) {
	case 2: return ((PPlane*)mbody2)->MathBody->Intersects(*MathBody);
	case 3: return ((PCube*)mbody2)->MathBody->Intersects(*MathBody);
	case 4: return ((PSphere*)mbody2)->MathBody->Intersects(*MathBody);
	case 5: return ((PCylinder*)mbody2)->MathBody->Intersects(*MathBody);
	default: return false;
	}

	return false;
}

bool PSphere::Intersects(Primitive* mbody2)
{
	switch (mbody2->GetType()) {
	case 2: return ((PPlane*)mbody2)->MathBody->Intersects(*MathBody);
	case 3: return ((PCube*)mbody2)->MathBody->Intersects(*MathBody);
	case 4: return ((PSphere*)mbody2)->MathBody->Intersects(*MathBody);
	case 5: return ((PCylinder*)mbody2)->MathBody->Intersects(*MathBody);
	default: return false;
	}

	return false;
}

bool PCylinder::Intersects(Primitive* mbody2)
{
	switch (mbody2->GetType()) {
	case 2: return ((PPlane*)mbody2)->MathBody->Intersects(*MathBody);
	case 3: return ((PCube*)mbody2)->MathBody->Intersects(*MathBody);
	case 4: return ((PSphere*)mbody2)->MathBody->Intersects(*MathBody);
	case 5: return ((PCylinder*)mbody2)->MathBody->Intersects(*MathBody);
	default: return false;
	}

	return false;
}