#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleUI.h"
#include "ConsoleWindow.h"
#include "SceneViewerWindow.h"
#include "ComponentCamera.h"
#include "ResourceMesh.h"
#include "KdTree.h"

#define MOV_SPEED 4.0f
#define MOUSE_SENSIBILITY 0.01f
#define MOUSE_WHEEL_SPEED 6.0f

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled, Type_Camera3D)
{
	background = Color(0.1f, 0.1f, 0.1f, 1.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	main_camera = new ComponentCamera(nullptr);

	App->ui->console_win->AddLog("Setting up the camera");
	bool ret = true;	

	main_camera->Move(vec(10.0f, 10.0f, 5.0f));
	main_camera->LookAt(vec(0.0f, 0.0f, 0.0f));

	App->eventSys->Subscribe(EventType::Window_Resize, this);

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	PLOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	if(App->ui->CheckDataWindows() || ImGui::IsMouseHoveringAnyWindow())
		return true;
	
	float speed = MOV_SPEED * dt * main_camera->mesh_multiplier;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = MOV_SPEED * 2 * dt * main_camera->mesh_multiplier;

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		if (App->input->GetKey(App->input->controls[MOVE_FORWARD]) == KEY_REPEAT) main_camera->MoveZ(speed);
		if (App->input->GetKey(App->input->controls[MOVE_BACK]) == KEY_REPEAT) main_camera->MoveZ(-speed);

		if (App->input->GetKey(App->input->controls[MOVE_LEFT]) == KEY_REPEAT) main_camera->MoveX(-speed);
		if (App->input->GetKey(App->input->controls[MOVE_RIGHT]) == KEY_REPEAT) main_camera->MoveX(speed);
	}
	if (App->input->GetMouseZ() < 0) main_camera->MoveZ(-speed * MOUSE_WHEEL_SPEED);
	if (App->input->GetMouseZ() > 0) main_camera->MoveZ(speed * MOUSE_WHEEL_SPEED);

	if (App->input->GetKey(App->input->controls[FOCUS_CAMERA]) == KEY_DOWN)
	{
		vec aux = vec(0.0f, 0.0f, 0.0f);

		for (int i = 0; i < App->gameObj->active_objects.size(); i++)
		{
			aux += App->gameObj->active_objects[i]->getObjectCenter();
		}
		
		if (App->gameObj->active_objects.size() > 0)
			aux = aux / App->gameObj->active_objects.size();

		if (App->gameObj->active_objects.size() > 0)
			main_camera->LookToActiveObjs(aux);

		else
			main_camera->LookAt(aux);
	}

	if (!ImGui::IsAnyWindowFocused() && App->input->GetKey(App->input->controls[ORBIT_CAMERA]) == KEY_IDLE && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver())
		MousePicking();

	if (App->input->GetKey(App->input->controls[ORBIT_CAMERA]) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		float3 aux = vec(0.0f, 0.0f, 0.0f);

		for (int i = 0; i < App->gameObj->active_objects.size(); i++)
		{
			aux += App->gameObj->active_objects[i]->getObjectCenter();
		}

		if (App->gameObj->active_objects.size() > 0)
			aux = aux / App->gameObj->active_objects.size();

		main_camera->RotateAround(aux);
	}
	else
	{
		main_camera->Reference = main_camera->frustum.pos;
	}
	// Mouse motion ----------------
	// TODO: Requires mouse reset properly without affecting MouseMotion
	// if want to get camera rotation without pressing Right Mouse Button
	// Or move mouse around the render windows while you are pressing then be able to continously drag around


	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		main_camera->Rotate();

	// Recalculate matrix -------------
	main_camera->CalculateViewMatrix();

	//DrawRay(picking.a, picking.b);

	return true;
}

bool ModuleCamera3D::Load(const JSON_Value * root_value)
{
	return true;
}

bool ModuleCamera3D::Save(JSON_Value* root_value)
{
	bool ret = false;

	JSON_Object* root_obj = json_value_get_object(root_value);

	json_serialize_to_file(root_value, "config_data.json");

	//App->ui->console_win->AddLog("Camera position saved");

	ret = true;
	return ret;
}

void ModuleCamera3D::MousePicking()
{
	float x = ((ImGui::GetMousePos().x / (float)App->window->window_w) * 2.0f) - 1.0f;
	float y = 1 - ((ImGui::GetMousePos().y / (float)App->window->window_h) * 2.0f);
	picking = (main_camera->frustum.UnProjectLineSegment(x,y));

	App->gameObj->SetActiveFalse();

	std::vector<GameObject*> intersected;
	std::vector<GameObject*> objects_to_check;

	if (App->gameObj->GetSceneKDTree() != nullptr)
		objects_to_check = App->gameObj->non_static_objects_in_scene;
	else
		objects_to_check = App->gameObj->objects_in_scene;

	if (App->gameObj->GetSceneKDTree() != nullptr)
		TreeTestIntersect(App->gameObj->GetSceneKDTree()->base_node, picking, objects_to_check);

	TestIntersect(objects_to_check, picking, intersected);

	float dist = INT_MAX;
	for (int i = 0; i < intersected.size(); i++)
	{
		float new_dist = TestTris(picking, intersected[i]->GetComponent(CT_Mesh)->AsMesh());
		if (new_dist < dist)
		{
			App->gameObj->SetActiveFalse();
			dist = new_dist;
			App->gameObj->active_objects.push_back(intersected[i]);
			intersected[i]->sv_active = true;
		}
	}

}

void ModuleCamera3D::TestIntersect(const std::vector<GameObject*>& objs, const LineSegment & ray, std::vector<GameObject*>& intersected)
{
	for (int i = 0; i < objs.size(); i++)
	{
		if (objs[i]->parent != nullptr && ray.ToRay().Intersects(*objs[i]->GetBB()))
		{
			Component* get = objs[i]->GetComponent(CT_Mesh);
			if (get != nullptr && get->active)
				intersected.push_back(objs[i]);
		}
	}
}

void ModuleCamera3D::TreeTestIntersect(const KDTree::Node* node, const LineSegment& ray, std::vector<GameObject*>& objects_to_check)
{
	if (ray.Intersects(node->bounding_box))
	{
		if (node->child.size() != 0)
		{
			for (int i = 0; i < node->child.size(); i++)
			{
				TreeTestIntersect(node->child[i], ray, objects_to_check);
			}
		}

		else
		{
			for (int i = 0; i < node->objects_in_node.size(); i++)
				objects_to_check.push_back(node->objects_in_node[i]);
		}
	}
}

float ModuleCamera3D::TestTris(LineSegment local, const ComponentMesh* mesh)
{
	float ret = INT_MAX;
	float4x4* global = &mesh->parent->GetTransform()->global_transform;
	local.Transform(global->Inverted());

	int count_hits = 0;
	ResourceMesh* r_mesh = mesh->r_mesh;

	for (int i = 0; i < mesh->r_mesh->num_index;)
	{
		vec vertex1 = { r_mesh->vertex[(r_mesh->index[i] * 3)], 
						r_mesh->vertex[(r_mesh->index[i] * 3) + 1],
						r_mesh->vertex[(r_mesh->index[i] * 3) + 2] }; 
		i++;

		vec vertex2 = { r_mesh->vertex[(r_mesh->index[i] * 3)], 
						r_mesh->vertex[(r_mesh->index[i] * 3) + 1],
						r_mesh->vertex[(r_mesh->index[i] * 3) + 2] }; 
		i++;

		vec vertex3 = { r_mesh->vertex[(r_mesh->index[i] * 3)], 
						r_mesh->vertex[(r_mesh->index[i] * 3) + 1],
						r_mesh->vertex[(r_mesh->index[i] * 3) + 2] }; 
		i++;
		Triangle test = Triangle(vertex1, vertex2, vertex3);
		vec intersect_point;
		bool check = local.ToRay().Intersects(test, nullptr, &intersect_point);
		float new_dist = (intersect_point - local.a).Length();
		if (check && new_dist < ret)
		{
			ret = new_dist;
			count_hits++;
		}
	}
	return ret;
}

void ModuleCamera3D::DrawRay(vec a, vec b) const
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3f(1.f, 1.f, 1.f);

	glVertex3f(a.x, a.y, a.z);
	glVertex3f(b.x, b.y, b.z);

	glEnd();

	if (App->renderer3D->lighting)
		glEnable(GL_LIGHTING);
}

void ModuleCamera3D::RecieveEvent(const Event & event)
{
	switch (event.type)
	{
	default:
		break;
	}
}

void ModuleCamera3D::SetMainCamAspectRatio()
{
	main_camera->SetAspectRatio();
}
