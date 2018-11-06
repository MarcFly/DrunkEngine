#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleUI.h"
#include "ConsoleWindow.h"
#include "Inspector.h"
#include "ComponentCamera.h"

#define MOV_SPEED 4.0f
#define MOUSE_SENSIBILITY 0.01f
#define MOUSE_WHEEL_SPEED 6.0f

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
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

	win_w = App->window->window_w;
	win_h = App->window->window_h;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	PLOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	if(ImGui::IsMouseHoveringAnyWindow() || App->ui->CheckDataWindows())
		return UPDATE_CONTINUE;

	float speed = MOV_SPEED * dt * main_camera->mesh_multiplier;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = MOV_SPEED * 2 * dt * main_camera->mesh_multiplier;

	if (App->input->GetKey(App->input->controls[MOVE_FORWARD]) == KEY_REPEAT) main_camera->MoveZ(speed);
	if (App->input->GetKey(App->input->controls[MOVE_BACK]) == KEY_REPEAT) main_camera->MoveZ(-speed);

	if (App->input->GetKey(App->input->controls[MOVE_LEFT]) == KEY_REPEAT) main_camera->MoveX(-speed);
	if (App->input->GetKey(App->input->controls[MOVE_RIGHT]) == KEY_REPEAT) main_camera->MoveX(speed);

	if (App->input->GetMouseZ() < 0) main_camera->MoveZ(-speed * MOUSE_WHEEL_SPEED);
	if (App->input->GetMouseZ() > 0) main_camera->MoveZ(speed * MOUSE_WHEEL_SPEED);

	if (App->input->GetKey(App->input->controls[FOCUS_CAMERA]) == KEY_DOWN)
	{
		vec aux = vec(0.0f, 0.0f, 0.0f);

		for (int i = 0; i < App->scene->active_objects.size(); i++)
		{
			aux += App->scene->active_objects[i]->getObjectCenter();
		}
		
		if (App->scene->active_objects.size() > 0)
			aux = aux / App->scene->active_objects.size();

		main_camera->LookAt(aux);
	}

	if (App->input->GetKey(App->input->controls[ORBIT_CAMERA]) == KEY_IDLE && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		MousePicking();

	if (App->input->GetKey(App->input->controls[ORBIT_CAMERA]) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		vec aux = vec(0.0f, 0.0f, 0.0f);

		for (int i = 0; i < App->scene->active_objects.size(); i++)
		{
			aux += App->scene->active_objects[i]->getObjectCenter();
		}

		if (App->scene->active_objects.size() > 0)
			aux = aux / App->scene->active_objects.size();

		main_camera->Reference = aux;

		main_camera->Rotate();
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

	SDL_GetWindowSize(App->window->window, &win_w, &win_h);
	if (win_w != App->window->window_w || win_h != App->window->window_h)
	{
		App->window->window_w = win_w;
		App->window->window_h = win_h;
		main_camera->SetAspectRatio();
		App->renderer3D->OnResize();
	}

	return UPDATE_CONTINUE;
}

bool ModuleCamera3D::Load(JSON_Value * root_value)
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
	float x = ((App->input->GetMouseX() / (float)App->window->window_w) * 2.0f) - 1.0f;
	float y = ((App->input->GetMouseY() / (float)App->window->window_h) * -2.0f) + 1.0f;
	LineSegment picking = (main_camera->frustum.UnProjectLineSegment(x,y));

	App->scene->active_objects.clear();
	App->ui->inspector->selection_mask_vec.clear();

	std::vector<GameObject*> intersected;

	TestIntersect(App->scene->Root_Object, picking, intersected);	

	float dist = INT_MAX;
	for (int i = 0; i < intersected.size(); i++)
	{
		float new_dist = TestTris(picking, intersected[i]->GetComponent(CT_Mesh)->AsMesh());
		if (new_dist < dist)
		{
			dist = new_dist;
			while(App->scene->active_objects.size() > 0)
				App->scene->active_objects.pop_back();
			App->scene->active_objects.push_back(intersected[i]);
		}
	}

}

void ModuleCamera3D::TestIntersect(GameObject * obj, LineSegment& ray, std::vector<GameObject*>& intersected)
{
	if (obj->isInsideFrustum(main_camera, obj->GetBB()))
		if (ray.Intersects(*obj->GetBB()))
		{
			if (obj->GetComponent(CT_Mesh) != nullptr)
				intersected.push_back(obj);

			for (int i = 0; i < obj->children.size(); i++)
				TestIntersect(obj->children[i], ray, intersected);
		}
}

float ModuleCamera3D::TestTris(LineSegment local, ComponentMesh* mesh)
{
	float ret = INT_MAX;
	float4x4* global = &mesh->parent->GetTransform()->global_transform;
	local.Transform(*global);

	for (int i = 0; i < mesh->num_faces / 3; i++)
	{
		vec vertex1 = { mesh->vertex[mesh->index[i * 3]], mesh->vertex[mesh->index[i * 3] + 1], mesh->vertex[mesh->index[i * 3] + 2] };
		vec vertex2 = { mesh->vertex[mesh->index[i * 3 + 1]], mesh->vertex[mesh->index[i * 3 + 1] + 1], mesh->vertex[mesh->index[i * 3 + 1] + 2] };
		vec vertex3 = { mesh->vertex[mesh->index[i * 3 + 2]], mesh->vertex[mesh->index[i * 3 + 2] + 1], mesh->vertex[mesh->index[i * 3 + 2] + 2] };
		Triangle test = Triangle(vertex1,vertex2,vertex3);
		float new_dist = INT_MAX;
		bool check = local.Intersects(test, &new_dist, nullptr);
		new_dist = vec(((vertex1 + vertex2 + vertex3) / 3.0f) - local.a).Length();
		if (check && new_dist < ret)
			ret = new_dist;
	}

	return ret;
}