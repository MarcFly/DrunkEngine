#include "Application.h"
#include "ModuleGameObject.h"
#include "ComponentCamera.h"
#include "KdTree.h"

ModuleGameObject::ModuleGameObject(bool start_enabled) : Module(start_enabled, Type_GameObj)
{
	mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	mCurrentGizmoMode = ImGuizmo::WORLD;
}

ModuleGameObject::~ModuleGameObject()
{
}

bool ModuleGameObject::Init()
{
	bool ret = true;

	

	return ret;
}

update_status ModuleGameObject::PreUpdate(float dt)
{

	

	return UPDATE_CONTINUE;
}

update_status ModuleGameObject::Update(float dt)
{
	ManageGuizmo();

	if (Root_Object != nullptr)
		Root_Object->Update(dt);

	if (Scene_KdTree != nullptr)
		Scene_KdTree->Update();

	return UPDATE_CONTINUE;
}

bool ModuleGameObject::CleanUp()
{
	bool ret = true;

	Scene_KdTree = nullptr;
	Main_Cam = nullptr;

	if (Root_Object != nullptr)
	{
		Root_Object->CleanUp();

		delete Root_Object;
		Root_Object = nullptr;
	}

	active_objects.clear();
	active_cameras.clear();
	objects_in_scene.clear();
	static_objects_in_scene.clear();
	non_static_objects_in_scene.clear();

	return ret;
}

void ModuleGameObject::Draw()
{

}

void ModuleGameObject::SetActiveFalse()
{
	for (int i = 0; i < active_objects.size(); i++)
	{
		active_objects[i]->active = false;
	}
	active_objects.clear();
}

void ModuleGameObject::SetmainCam(ComponentCamera * cam)
{
	Main_Cam = cam;
}

void ModuleGameObject::NewScene()
{
	DeleteScene();

	Root_Object = new GameObject();
	Root_Object->name = "NewScene";
}

void ModuleGameObject::CreateMainCam()
{
	if (active_cameras.size() < 1)
	{
		GameObject* MainCam = new GameObject(Root_Object, "Main Camera", CT_Camera);
		getRootObj()->children.push_back(MainCam);
	}
}

void ModuleGameObject::DeleteScene()
{
	if (getRootObj() != nullptr)
		getRootObj()->DestroyThisObject();
	delete Root_Object;
}

void ModuleGameObject::SetSceneObjects(GameObject * obj)
{
	if (obj->parent == nullptr)
		objects_in_scene.clear();
	
	objects_in_scene.push_back(obj);

	for (int i = 0; i < obj->children.size(); i++)
	{
		SetSceneObjects(obj->children[i]);
	}
}

void ModuleGameObject::SetToStaticObjects(GameObject * obj)
{
	static_objects_in_scene.push_back(obj);

	for (int i = 0; i < obj->children.size(); i++)
	{
		SetToStaticObjects(obj->children[i]);
	}

	SetNonStaticList();
}

void ModuleGameObject::DeleteFromStaticObjects(GameObject * obj)
{
	for (int i = 0; i < static_objects_in_scene.size(); i++)
	{
		if (static_objects_in_scene[i] == obj)
		{
			static_objects_in_scene[i]->static_pop = true;
			AdjustStaticList();
			break;
		}
	}

	for (int i = 0; i < obj->children.size(); i++)
	{
		DeleteFromStaticObjects(obj->children[i]);
	}

	SetNonStaticList();
}

void ModuleGameObject::AdjustStaticList()
{
	int i = 0;
	for (; i < static_objects_in_scene.size(); i++)
	{
		if (static_objects_in_scene[i]->static_pop == true)
		{
			static_objects_in_scene[i]->static_pop = false;
			static_objects_in_scene[i] = nullptr;
			break;
		}
	}

	for (int j = i; j < static_objects_in_scene.size() - 1; j++)
	{
		static_objects_in_scene[j] = static_objects_in_scene[j + 1];
	}

	static_objects_in_scene.pop_back();
}

void ModuleGameObject::SetNonStaticList()
{
	non_static_objects_in_scene.clear();

	for (int i = 0; i < objects_in_scene.size(); i++)
	{
		if (!objects_in_scene[i]->is_static)
			non_static_objects_in_scene.push_back(objects_in_scene[i]);
	}
}

void ModuleGameObject::SetSceneKDTree(const int elements_per_node, const int max_subdivisions)
{
	Scene_KdTree = new KDTree(elements_per_node, max_subdivisions);
}

KDTree * ModuleGameObject::GetSceneKDTree() const
{
	return Scene_KdTree;
}

void ModuleGameObject::DeleteSceneKDTree()
{
	Scene_KdTree->CleanUp();
	Scene_KdTree = nullptr;
}

void ModuleGameObject::SetRootObject(GameObject * root)
{
	Root_Object = root;
}

GameObject * ModuleGameObject::GetRootObject() const
{
	return Root_Object;
}

void ModuleGameObject::ManageGuizmo()
{

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
			mCurrentGizmoOperation = ImGuizmo::SCALE;
	}

	

	for (int i = 0; i < active_objects.size(); i++)
	{
		if (!active_objects[i]->is_static)
		{
			float aux_vals[16];
			float4x4 aux_mat = active_objects[i]->GetTransform()->global_transform;
			
			ImGuizmo::SetRect(0, 0, App->window->screen_size_w, App->window->screen_size_h);
			ImGuizmo::Manipulate(Main_Cam->GetViewMatrix(), Main_Cam->frustum.ProjectionMatrix().ptr(), mCurrentGizmoOperation, mCurrentGizmoMode, aux_mat.ptr(), aux_vals);

			aiMatrix4x4 new_transform = aiMatrix4x4(aux_vals[0], aux_vals[1], aux_vals[2], aux_vals[3],
													aux_vals[4], aux_vals[5], aux_vals[6], aux_vals[7],
													aux_vals[8], aux_vals[9], aux_vals[10], aux_vals[11],
													aux_vals[12], aux_vals[13], aux_vals[14], aux_vals[15]);

			if (ImGuizmo::IsUsing())
			{
				aiVector3D scale;
				aiVector3D pos;
				aiQuaternion rot;

				new_transform.Decompose(scale, rot, pos);

				switch (mCurrentGizmoOperation)
				{
				case ImGuizmo::TRANSLATE:
				{
					active_objects[i]->GetTransform()->SetTransformPosition(pos.x, pos.y, pos.x);
					break;
				}
				case ImGuizmo::ROTATE:
				{
					Quat rotate_quat = Quat(rot.x, rot.y, rot.z, rot.w);
					active_objects[i]->GetTransform()->SetTransformRotation(rotate_quat);
					break;
				}
				case ImGuizmo::SCALE:
				{
					float3 scale_float3 = float3(scale.x, scale.y, scale.z);
					active_objects[i]->GetTransform()->SetTransformScale(scale_float3.x, scale_float3.y, scale_float3.x);
					break;
				}
				}
				
			}
		}
	}
}
