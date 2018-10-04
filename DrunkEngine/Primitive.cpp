
#include "Globals.h"
#include "GLEW/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Primitive.h"
#include "SDL/include/SDL_opengl.h"

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

		glTexCoord2f(0.f, 0.f);
		glTexCoord2f(1.f, 1.f);
		glTexCoord2f(0.f, 1.f);

		glVertex3f(MathBody->CornerPoint(0).x, MathBody->CornerPoint(0).y, MathBody->CornerPoint(0).z);
		glVertex3f(MathBody->CornerPoint(1).x, MathBody->CornerPoint(1).y, MathBody->CornerPoint(1).z);
		glVertex3f(MathBody->CornerPoint(3).x, MathBody->CornerPoint(3).y, MathBody->CornerPoint(3).z);
		
		glTexCoord2f(0.f, 0.f);
		glTexCoord2f(1.f, 0.f);
		glTexCoord2f(1.f, 1.f);

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

		const int CHECKERS_WIDTH = 2;
		const int CHECKERS_HEIGHT = 2;

		GLubyte checkImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
		for (int i = 0; i < CHECKERS_HEIGHT; i++) {
			for (int j = 0; j < CHECKERS_WIDTH; j++) {
				int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
				checkImage[i][j][0] = (GLubyte)c;
				checkImage[i][j][1] = (GLubyte)c;
				checkImage[i][j][2] = (GLubyte)c;
				checkImage[i][j][3] = (GLubyte)255;
			}
		}

		uint ImageName = 1;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &ImageName);
		glBindTexture(GL_TEXTURE_2D, ImageName);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
			0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

	}


	
	// Draw from array
	/*
	{
		// Prep Order
		std::vector<int> vert_order = 
			{	0,3,2,	0,1,3,
				1,5,7,	1,7,3,
				5,4,7,	4,6,7,
				4,0,2,	4,2,6,
				2,3,7,	2,7,6,
				0,4,5,	0,5,1
			};

		// Prep Draw Array
		std::vector<vec> draw_verts;
		for (int i = 0; i < vert_order.size(); i++)
		{
			draw_verts.push_back(MathBody->CornerPoint(vert_order[i]));
		}

		uint buff_id = 0;

		glEnableClientState(GL_VERTEX_ARRAY);

		glGenBuffers(1, &buff_id1);
		glBindBuffer(GL_ARRAY_BUFFER, buff_id1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*draw_verts.size()*3, &draw_verts[0], GL_STATIC_DRAW);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		glDrawArrays(GL_TRIANGLES, 0, draw_verts.size());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	*/
	// Draw from array, elementwise (quite the same fucking thing)
/*
	{
		// Prep Order
		std::vector<int> vert_order =
		{	0,3,2,	0,1,3,
			1,5,7,	1,7,3,
			5,4,7,	4,6,7,
			4,0,2,	4,2,6,
			2,3,7,	2,7,6,
			0,4,5,	0,5,1
		};

		// Prep Draw Array
		std::vector<vec> vert_array;
		for (int i = 0; i < 8; i++)
		{
			vert_array.push_back(MathBody->CornerPoint(i));
		}

		uint buff_id = 0;

		glEnableClientState(GL_VERTEX_ARRAY);

		glGenBuffers(1, &buff_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buff_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*vert_order.size(), &vert_order[0], GL_STATIC_DRAW);
		glVertexPointer(3, GL_FLOAT, 0, &vert_array[0]);
		glDrawElements(GL_TRIANGLES, vert_order.size(),GL_UNSIGNED_INT, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	*/
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
	// Direct Sphere Rendering

	// Sector Count and Stack Count are the arbitrary resolutions of this Direct Sphere
	/*float sectorCount = 4;
	float stackCount = 2;

	// clear memory of prev arrays
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texCoords;

	float4 vert_pos;	//	vertex position and w = xy =  r * cos(u)
	float4 normal = { 0,0,0, 1.0f / MathBody->r };		//	vertex normals, w = lengthInv = 1.0f / radius
	float s, t;			// vertex texCoord

	float sectorStep = 2 * pi / sectorCount;
	float stackStep = pi / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = pi / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		vert_pos.w = radius * cosf(stackAngle);             // r * cos(u)
		vert_pos.z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coods
		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;

			// vertex position (x, y, z)
			vert_pos.x = vert_pos.w * cosf(sectorAngle);             // r * cos(u) * cos(v)
			vert_pos.y = vert_pos.w * sinf(sectorAngle);             // r * cos(u) * sin(v)
			vertices.push_back(vert_pos.x);
			vertices.push_back(vert_pos.y);
			vertices.push_back(vert_pos.z);

			// vertex normal (nx, ny, nz)
			normal.x = vert_pos.x * normal.w;
			normal.y = vert_pos.y * normal.w;
			normal.z = vert_pos.z * normal.w;
			normals.push_back(normal.x);
			normals.push_back(normal.y);
			normals.push_back(normal.z);

			// vertex tex coord (s, t)
			s = (float)j / sectorCount;
			t = (float)i / stackCount;
			texCoords.push_back(s);
			texCoords.push_back(t);
		}
	}

	// generate index list of sphere triangles
	std::vector<int> indices;
	int k1, k2;
	for (int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding 1st and last stacks
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != (stackCount - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	

	// Geosphere Rendering
	
	const float H_ANGLE = pi / 180 * 72;    // 72 degree = 360 / 5
	const float V_ANGLE = atanf(1.0f / 2);  // elevation = 26.565 degree

	vec vertices[12];			// array of 12 vertices (x,y,z)
	int i1, i2;                             // indices
	float z, xy;                            // coords
	float hAngle1 = -pi / 2 - H_ANGLE / 2;  // start from -126 deg at 1st row
	float hAngle2 = -pi / 2;                // start from -90 deg at 2nd row

	// the first top vertex at (0, 0, r)
	vertices[0].x = 0;
	vertices[0].y = 0;
	vertices[0].z = radius;

	// compute 10 vertices at 1st and 2nd rows
	for (int i = 1; i <= 5; ++i)
	{
		i1 = i;         // for 1st row
		i2 = (i + 5);   // for 2nd row

		z = radius * sinf(V_ANGLE);				// elevaton
		xy = radius * cosf(V_ANGLE);            // length on XY plane

		vertices[i].x = xy * cosf(hAngle1);      // x
		vertices[i + 5].x = xy * cosf(hAngle2);
		vertices[i].y = xy * sinf(hAngle1);		// y
		vertices[i + 5].y = xy * sinf(hAngle2);
		vertices[i].z = z;						// z
		vertices[i + 5].z = -z;

		// next horizontal angles
		hAngle1 += H_ANGLE;
		hAngle2 += H_ANGLE;
	}

	// the last bottom vertex at (0, 0, -r)
	vertices[11].x = 0;
	vertices[11].y = 0;
	vertices[11].z = -radius;
	

	// Rendering sphere

	// copy interleaved vertex data (V/N/T) to VBO
	GLuint vboId;
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER,                   // target
		sizeof(float) * vertices.size(), // data size, bytes
		&vertices[0],   // ptr to vertex data
		GL_STATIC_DRAW);                   // usage

	// copy index data to VBO
	GLuint iboId;
	glGenBuffers(1, &iboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
		indices.size(),             // data size, bytes
		&indices[0],               // ptr to index data
		GL_STATIC_DRAW);                   // usage

		// draw sphere with VBO
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);

	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(1);

	int stride = 32;     // should be 32 bytes
	glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));
	glVertexAttribPointer(1, 2, GL_FLOAT, false, stride, (void*)(sizeof(float) * 6));

	glDrawElements(GL_TRIANGLES,                    // primitive type
		indices.size(),          // # of indices
		GL_UNSIGNED_INT,                 // data type
		(void*)0);                       // offset to indices

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(1);

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	*/
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