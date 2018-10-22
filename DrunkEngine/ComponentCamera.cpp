#include "ComponentCamera.h"
#include "GameObject.h"
#include "Application.h"

ComponentCamera::ComponentCamera(GameObject * par)
{
	this->parent = par;

	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.front = float3(0.f, 0.f, 1.f);
	frustum.up = float3(0.f, 1.f, 0.f);

	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 100.0f;

	frustum.verticalFov = DegToRad(60.0f);
	SetAspectRatio();

	frustum.pos = float3::zero;

	frustum.GetCornerPoints(bb_frustum);
}

ComponentCamera::~ComponentCamera()
{
}

void ComponentCamera::Start()
{
}

bool ComponentCamera::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_REPEAT)
	{
		frustum.verticalFov += 0.0015;
		frustum.GetCornerPoints(bb_frustum);
	}
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_REPEAT)
	{
		frustum.verticalFov -= 0.0015;
		frustum.GetCornerPoints(bb_frustum);
	}

	SetAspectRatio();

	return true;
}

void ComponentCamera::Draw()
{
	glDisable(GL_LIGHTING);
	frustum.verticalFov;
	glBegin(GL_LINES);
	glColor3f(1.f, 1.f, 1.f);

	glVertex3f((GLfloat)bb_frustum[0].x, (GLfloat)bb_frustum[0].y, (GLfloat)bb_frustum[0].z);
	glVertex3f((GLfloat)bb_frustum[1].x, (GLfloat)bb_frustum[1].y, (GLfloat)bb_frustum[1].z);

	glVertex3f((GLfloat)bb_frustum[0].x, (GLfloat)bb_frustum[0].y, (GLfloat)bb_frustum[0].z);
	glVertex3f((GLfloat)bb_frustum[4].x, (GLfloat)bb_frustum[4].y, (GLfloat)bb_frustum[4].z);

	glVertex3f((GLfloat)bb_frustum[0].x, (GLfloat)bb_frustum[0].y, (GLfloat)bb_frustum[0].z);
	glVertex3f((GLfloat)bb_frustum[2].x, (GLfloat)bb_frustum[2].y, (GLfloat)bb_frustum[2].z);

	glVertex3f((GLfloat)bb_frustum[2].x, (GLfloat)bb_frustum[2].y, (GLfloat)bb_frustum[2].z);
	glVertex3f((GLfloat)bb_frustum[3].x, (GLfloat)bb_frustum[3].y, (GLfloat)bb_frustum[3].z);

	glVertex3f((GLfloat)bb_frustum[1].x, (GLfloat)bb_frustum[1].y, (GLfloat)bb_frustum[1].z);
	glVertex3f((GLfloat)bb_frustum[3].x, (GLfloat)bb_frustum[3].y, (GLfloat)bb_frustum[3].z);

	glVertex3f((GLfloat)bb_frustum[5].x, (GLfloat)bb_frustum[5].y, (GLfloat)bb_frustum[5].z);
	glVertex3f((GLfloat)bb_frustum[4].x, (GLfloat)bb_frustum[4].y, (GLfloat)bb_frustum[4].z);

	glVertex3f((GLfloat)bb_frustum[4].x, (GLfloat)bb_frustum[4].y, (GLfloat)bb_frustum[4].z);
	glVertex3f((GLfloat)bb_frustum[6].x, (GLfloat)bb_frustum[6].y, (GLfloat)bb_frustum[6].z);

	glVertex3f((GLfloat)bb_frustum[7].x, (GLfloat)bb_frustum[7].y, (GLfloat)bb_frustum[7].z);
	glVertex3f((GLfloat)bb_frustum[5].x, (GLfloat)bb_frustum[5].y, (GLfloat)bb_frustum[5].z);

	glVertex3f((GLfloat)bb_frustum[7].x, (GLfloat)bb_frustum[7].y, (GLfloat)bb_frustum[7].z);
	glVertex3f((GLfloat)bb_frustum[6].x, (GLfloat)bb_frustum[6].y, (GLfloat)bb_frustum[6].z);

	glVertex3f((GLfloat)bb_frustum[1].x, (GLfloat)bb_frustum[1].y, (GLfloat)bb_frustum[1].z);
	glVertex3f((GLfloat)bb_frustum[5].x, (GLfloat)bb_frustum[5].y, (GLfloat)bb_frustum[5].z);

	glVertex3f((GLfloat)bb_frustum[6].x, (GLfloat)bb_frustum[6].y, (GLfloat)bb_frustum[6].z);
	glVertex3f((GLfloat)bb_frustum[2].x, (GLfloat)bb_frustum[2].y, (GLfloat)bb_frustum[2].z);

	glVertex3f((GLfloat)bb_frustum[7].x, (GLfloat)bb_frustum[7].y, (GLfloat)bb_frustum[7].z);
	glVertex3f((GLfloat)bb_frustum[3].x, (GLfloat)bb_frustum[3].y, (GLfloat)bb_frustum[3].z);
	
	glEnd();

	if (App->renderer3D->lighting)
		glEnable(GL_LIGHTING);
}

void ComponentCamera::CleanUp()
{
}

void ComponentCamera::SetAspectRatio()
{
	float aspect_ratio = ((float)App->window->window_w / (float)App->window->window_h);		//Window aspect ratio
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspect_ratio);
	projection_update = true;
}

bool ComponentCamera::Load(JSON_Value * root_value)
{
	return false;
}

bool ComponentCamera::Save(JSON_Value * root_value)
{
	return false;
}
