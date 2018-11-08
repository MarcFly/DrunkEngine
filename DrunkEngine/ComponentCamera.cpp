#include "ComponentCamera.h"
#include "GameObject.h"
#include "Application.h"

#define MOV_SPEED 4.0f
#define MOUSE_SENSIBILITY 0.01f
#define MOUSE_WHEEL_SPEED 6.0f

ComponentCamera::ComponentCamera(GameObject * par)
{
	SetBaseVals();

	if (par != nullptr)
	{
		this->parent = par;
		App->gameObj->active_cameras.push_back(this);
	}
	else
		App->gameObj->SetmainCam(this);

	CalculateViewMatrix();

	id = App->gameObj->active_cameras.size();

	X = vec(1.0f, 0.0f, 0.0f);
	Y = vec(0.0f, 1.0f, 0.0f);
	Z = vec(0.0f, 0.0f, 1.0f);

	Reference = vec(0.0f, 0.0f, 0.0f);

	frustum.nearPlaneDistance = 0.5f;
	frustum.farPlaneDistance = 1000.0f;

	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.projectiveSpace = FrustumProjectiveSpace::FrustumSpaceGL;
	frustum.handedness = FrustumHandedness::FrustumRightHanded;

	frustum.SetWorldMatrix(float3x4::identity);

	frustum.verticalFov = DegToRad(60.0f);
	SetAspectRatio();

	frustum.SetPerspective(frustum.horizontalFov, frustum.verticalFov);

	frustum.pos = float3::zero;

	frustum.Translate(vec(0.0f, 0.0f, 5.0f));

	SetbbFrustum();

	// If scale is compatible whith camera

	//original_v_fov = frustum.verticalFov;
	//original_h_fov = frustum.horizontalFov;

	mesh_multiplier = 1;

}

ComponentCamera::~ComponentCamera()
{
}

void ComponentCamera::Start()
{
	// Start
}

void ComponentCamera::Update(const float dt)
{
	float speed = MOV_SPEED * dt;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 2 * MOV_SPEED * dt;

	if (parent->GetTransform()->update_camera_transform)
	{
		TransformPos(parent->GetTransform()->position);
		TransformRot(parent->GetTransform()->rotate_quat);
		TransformScale(parent->GetTransform()->scale);

		SetbbFrustum();

		parent->GetTransform()->update_camera_transform = false;
	}

	// To rotate Component Cameras (not editor camera)
	//if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	//{
	//	Rotate();
	//	SetbbFrustum();
	//}

	CalculateViewMatrix();
}

void ComponentCamera::Draw()
{
	if (parent != nullptr)
	{
		glDisable(GL_LIGHTING);
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
}

void ComponentCamera::CleanUp()
{
	// Cleanup
}

// -----------------------------------------------------------------
void ComponentCamera::LookAt(const vec &spot)
{
	float3 look_to = spot - frustum.pos;

	float3x3 direction_matrix = float3x3::LookAt(frustum.Front(), look_to.Normalized(), frustum.Up(), float3::unitY);

	frustum.SetFront(direction_matrix.MulDir(frustum.Front()).Normalized());
	frustum.SetUp(direction_matrix.MulDir(frustum.Up()).Normalized());
}

// -----------------------------------------------------------------
void ComponentCamera::Move(const vec &Movement)
{
	Reference += Movement;

	if (Movement.x < 0)
	{
		float3 aux = float3::zero;
		aux = frustum.Front() * -Movement.x;
		frustum.Translate(aux);
	}
	if (Movement.x > 0)
	{
		float3 aux = float3::zero;
		aux = frustum.Front() * Movement.x;
		frustum.Translate(aux);
	}
	if (Movement.y < 0)
	{
		float3 aux = float3::zero;
		aux = frustum.Front() * -Movement.y;
		frustum.Translate(aux);
	}
	if (Movement.y > 0)
	{
		float3 aux = float3::zero;
		aux = frustum.Front() * Movement.y;
		frustum.Translate(aux);
	}
	if (Movement.z < 0)
	{
		float3 aux = float3::zero;
		aux = frustum.WorldRight() * -Movement.z;
		frustum.Translate(aux);
	}
	if (Movement.z > 0)
	{
		float3 aux = float3::zero;
		aux = frustum.WorldRight() * Movement.z;
		frustum.Translate(aux);
	}
}

void ComponentCamera::MoveZ(const float mov)
{
	if (mov < 0)
	{
		float3 aux = float3::zero;
		aux = frustum.Front() * mov;
		frustum.Translate(aux);
		frustum.SetFront(frustum.Front());
	}
	if (mov > 0)
	{
		float3 aux = float3::zero;
		aux = frustum.Front() * mov;
		frustum.Translate(aux);
		frustum.SetFront(frustum.Front());
	}
}

void ComponentCamera::MoveX(const float mov)
{
	if (mov < 0)
	{
		float3 aux = float3::zero;
		aux = frustum.WorldRight() * mov;
		frustum.Translate(aux);
		frustum.SetUp(frustum.Up());
	}
	if (mov > 0)
	{
		float3 aux = float3::zero;
		aux = frustum.WorldRight() * mov;
		frustum.Translate(aux);
		frustum.SetUp(frustum.Up());
	}
}

// -----------------------------------------------------------------
void ComponentCamera::Transport(const vec &Movement)
{
	frustum.pos = Movement;

	CalculateViewMatrix();
}

void ComponentCamera::Rotate()
{
	float dx = -App->input->GetMouseXMotion() * MOUSE_SENSIBILITY;
	float dy = -App->input->GetMouseYMotion() * MOUSE_SENSIBILITY;

	Quat rot_x = Quat::RotateY(dx);
	frustum.SetFront(rot_x.Mul(frustum.Front()).Normalized());
	frustum.SetUp(rot_x.Mul(frustum.Up()).Normalized());

	Quat rot_y = Quat::RotateAxisAngle(frustum.WorldRight(), dy);
	frustum.SetFront(rot_y.Mul(frustum.Front()).Normalized());
	frustum.SetUp(rot_y.Mul(frustum.Up()).Normalized());
}

float3 ComponentCamera::RotateAngle(const float3 &u, float angle, const float3 &v)
{
	return *(float3*)&(float4x4::RotateAxisAngle(v, angle) * float4(u, 1.0f));
}

void ComponentCamera::SetAspectRatio()
{
	float aspect_ratio = ((float)App->window->window_w / (float)App->window->window_h);		//Window aspect ratio
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspect_ratio);
	frustum.SetPerspective(frustum.horizontalFov, frustum.verticalFov);
}

float * ComponentCamera::GetViewMatrix()
{
	return &ViewMatrix[0][0];
}

void ComponentCamera::CalculateViewMatrix()
{
	ViewMatrix = this->frustum.ViewMatrix();
	ViewMatrix = ViewMatrix.Transposed();
	ViewMatrixInverse = ViewMatrix.Inverted();
}

void ComponentCamera::LookToObj(GameObject* obj, float vertex_aux)
{
	Transport(vec(vertex_aux + 3, vertex_aux + 3, vertex_aux + 3));

	vec aux = obj->getObjectCenter();
	LookAt(vec(aux.x, aux.y, aux.z));

	mesh_multiplier = vertex_aux / 4 + 1;
}

void ComponentCamera::LookToActiveObjs(vec look_to)
{
	float aux = 0;

	for (int i = 0; i < App->gameObj->active_objects.size(); i++)
	{
		vec test_max = App->gameObj->active_objects[i]->BoundingBox->maxPoint;
		vec test_min = App->gameObj->active_objects[i]->BoundingBox->minPoint;

		vec test = test_max - test_min;

		if (aux < abs(test.x))
			aux = abs(test.x);

		if (aux < abs(test.y))
			aux = abs(test.y);

		if (aux < abs(test.z))
			aux = abs(test.z);
	}

	Transport(vec(aux + 3, aux + 3, aux + 3));

	LookAt(look_to);

	if (aux != 0)
		mesh_multiplier = aux / 4;
	else
		mesh_multiplier = 1;
}

void ComponentCamera::TransformPos(float3 pos)
{
	frustum.pos = pos;
}

void ComponentCamera::TransformRot(Quat rot)
{

	float3 new_rot = rot.ToEulerXYZ();

	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);

	Quat rotation_y = Quat::RotateY(new_rot.y);
	frustum.SetFront(rotation_y.Mul(frustum.Front()).Normalized());
	frustum.SetUp(rotation_y.Mul(frustum.Up()).Normalized());

	Quat rotation_x = Quat::RotateAxisAngle(frustum.WorldRight(), new_rot.x);
	frustum.SetFront(rotation_x.Mul(frustum.Front()).Normalized());
	frustum.SetUp(rotation_x.Mul(frustum.Up()).Normalized());

	Quat rotation_z = Quat::RotateZ(new_rot.z);
	frustum.SetFront(rotation_z.Mul(frustum.Front()).Normalized());
	frustum.SetUp(rotation_z.Mul(frustum.Up()).Normalized());
}

void ComponentCamera::TransformScale(float3 scale)
{
	//frustum.verticalFov = DegToRad(RadToDeg(original_v_fov) * scale.y);
	//frustum.horizontalFov = DegToRad(RadToDeg(original_h_fov) * scale.x);
}

void ComponentCamera::SetbbFrustum()
{
	frustum.GetCornerPoints(bb_frustum);
}

void ComponentCamera::Load(JSON_Object* comp)
{

}

void ComponentCamera::Save(JSON_Array* comps)
{

}
