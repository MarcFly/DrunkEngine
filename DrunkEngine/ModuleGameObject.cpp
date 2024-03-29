#include "Application.h"
#include "ModuleGameObject.h"
#include "ComponentCamera.h"
#include "KdTree.h"
#include "InspectorWindow.h"
#include "ConsoleWindow.h"

ModuleGameObject::ModuleGameObject(bool start_enabled) : Module(start_enabled, Type_GameObj)
{
	mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	mCurrentGizmoMode = ImGuizmo::WORLD;
	on_click_scale = float3::one;
	on_click_pos = float3::zero;
}

ModuleGameObject::~ModuleGameObject()
{
}

bool ModuleGameObject::Init()
{
	bool ret = true;

	App->eventSys->Subscribe(EventType::Transform_Updated, this);
	App->eventSys->Subscribe(EventType::Update_Cam_Focus, this);

	return ret;
}

bool ModuleGameObject::PreUpdate(float dt)
{

	return true;
}

bool ModuleGameObject::Update(float dt)
{
	ManageGuizmo();

	if (Scene_KdTree != nullptr && Scene_KdTree->base_node != nullptr)
		Scene_KdTree->CheckKDTreeInsideFrustum(Scene_KdTree->base_node, camera_rendering);

	if (Root_Object != nullptr)
		Root_Object->Update(dt);

	if (Scene_KdTree != nullptr)
		Scene_KdTree->Update();

	return true;
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

void ModuleGameObject::SetActiveFalse()
{
	for (int i = 0; i < active_objects.size(); i++)
	{
		active_objects[i]->sv_active = false;
	}
	active_objects.clear();
}

void ModuleGameObject::SetmainCam(ComponentCamera * cam)
{
	Main_Cam = cam;
}

void ModuleGameObject::NewScene(GameObject* base)
{
	DeleteScene();

	if (base == nullptr)
	{
		Root_Object = new GameObject();
		Root_Object->name = "NewScene";
		Root_Object->UUID = UINT_FAST32_MAX;
	}
	else
		Root_Object = base;

	SetActiveFalse();
}

void ModuleGameObject::CreateMainCam()
{
	if (active_cameras.size() < 1)
	{
		GameObject* Game_camera = new GameObject("Main Camera", Root_Object);
		Game_camera->components.push_back(new ComponentCamera(Game_camera));
		getRootObj()->children.push_back(Game_camera);
		Game_camera->GetTransform()->SetTransformPosition(0.f, 0.f, -100.f);
		
		Event evTrans(EventType::Transform_Updated, Event::UnionUsed::UseGameObject);
		evTrans.game_object.ptr = Game_camera;
		App->eventSys->BroadcastEvent(evTrans);
	}
}

void ModuleGameObject::DeleteScene()
{
	objects_in_scene.clear();
	static_objects_in_scene.clear();
	non_static_objects_in_scene.clear();

	active_cameras.clear();

	SetCameraRender(Main_Cam);
	App->ui->inspector_win->main_cam = false;

	if (getRootObj() != nullptr)
	{
		getRootObj()->DestroyThisObject();
		delete Root_Object;
	}
}

void ModuleGameObject::SetSceneObjects(GameObject * obj)
{
	if (obj == nullptr)
		obj = getRootObj();

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
	if (Scene_KdTree != nullptr)
	{
		Scene_KdTree->CleanUp();
		Scene_KdTree = nullptr;
		App->ui->console_win->AddLog("KD-Tree Deleted.");
	}
}

void ModuleGameObject::SetRootObject(GameObject * root)
{
	Root_Object = root;
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
		if (!active_objects[i]->is_static && active_objects[i] != Root_Object)
		{
			float aux_vals[16];
			float4x4 aux_mat;

			if (mCurrentGizmoMode == ImGuizmo::LOCAL || mCurrentGizmoOperation == ImGuizmo::SCALE || active_objects[i]->parent == nullptr)
				aux_mat = active_objects[i]->GetTransform()->global_transform.Transposed();

			else
				aux_mat = active_objects[i]->GetTransform()->aux_world_pos.Transposed();

			ImGuizmo::SetRect(0, 0, App->window->window_w, App->window->window_h);
			ImGuizmo::Manipulate(Main_Cam->GetViewMatrix(), Main_Cam->frustum.ProjectionMatrix().ptr(), mCurrentGizmoOperation, mCurrentGizmoMode, aux_mat.ptr(), aux_vals);

			aiMatrix4x4 new_transform = aiMatrix4x4(aux_vals[0], aux_vals[1], aux_vals[2], aux_vals[3],
													aux_vals[4], aux_vals[5], aux_vals[6], aux_vals[7],
													aux_vals[8], aux_vals[9], aux_vals[10], aux_vals[11],
													aux_vals[12], aux_vals[13], aux_vals[14], aux_vals[15]);

			if (mCurrentGizmoOperation == ImGuizmo::SCALE && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && ImGuizmo::IsOver())
				on_click_scale = active_objects[i]->GetTransform()->scale;

			if (mCurrentGizmoOperation == ImGuizmo::TRANSLATE && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && ImGuizmo::IsOver())
				on_click_pos = active_objects[i]->GetTransform()->position;

			if (ImGuizmo::IsUsing() && App->input->GetKey(App->input->controls[ORBIT_CAMERA]) != KEY_REPEAT)
			{
				aiVector3D scale;
				aiVector3D pos;
				aiQuaternion rot;

				new_transform.Transpose();
				new_transform.Decompose(scale, rot, pos);

				switch (mCurrentGizmoOperation)
				{
				case ImGuizmo::TRANSLATE:
				{
					if (mCurrentGizmoMode == ImGuizmo::WORLD)
					{
						float4x4 new_transform = float4x4::FromTRS(float3(pos.x, pos.y, pos.z), Quat::identity, float3::one);
						active_objects[i]->GetTransform()->SetWorldPos(new_transform);

						Event ev(EventType::Transform_Updated, Event::UnionUsed::UseGameObject);
						ev.game_object.ptr = active_objects[i];
						App->eventSys->BroadcastEvent(ev);

						break;
					}
					else	// LOCAL
					{
						float3 pos_float3 = float3(pos.x, pos.y, pos.z) + on_click_pos;
						active_objects[i]->GetTransform()->SetTransformPosition(pos_float3.x, pos_float3.y, pos_float3.z);
						
						Event ev(EventType::Transform_Updated, Event::UnionUsed::UseGameObject);
						ev.game_object.ptr = active_objects[i];
						App->eventSys->BroadcastEvent(ev);

						break;
					}
				}
				case ImGuizmo::ROTATE:
				{
					if (mCurrentGizmoMode == ImGuizmo::WORLD)
					{
						active_objects[i]->GetTransform()->SetWorldRot(Quat(rot.x, rot.y, rot.z, rot.w));

						Event ev(EventType::Transform_Updated, Event::UnionUsed::UseGameObject);
						ev.game_object.ptr = active_objects[i];
						App->eventSys->BroadcastEvent(ev);

						break;
					}
					else	// LOCAL
					{
						Quat rotate_quat = Quat(rot.x, rot.y, rot.z, rot.w).Mul(active_objects[i]->GetTransform()->rotate_quat);
						active_objects[i]->GetTransform()->SetTransformRotation(rotate_quat);

						Event ev(EventType::Transform_Updated, Event::UnionUsed::UseGameObject);
						ev.game_object.ptr = active_objects[i];
						App->eventSys->BroadcastEvent(ev);

						break;
					}
				}
				case ImGuizmo::SCALE:
				{
					//Only Local
					float3 scale_float3 = float3(scale.x - 1, scale.y - 1, scale.z - 1).Mul(on_click_scale);
					active_objects[i]->GetTransform()->SetTransformScale(on_click_scale.x + scale_float3.x, on_click_scale.y + scale_float3.y, on_click_scale.z + scale_float3.z);

					Event ev(EventType::Transform_Updated, Event::UnionUsed::UseGameObject);
					ev.game_object.ptr = active_objects[i];
					App->eventSys->BroadcastEvent(ev);

					break;
				}
				}

			}
		}
	}
}

void ModuleGameObject::SetCameraRender(ComponentCamera * cam)
{
	camera_rendering = cam;
}

bool ModuleGameObject::isInsideFrustum(const ComponentCamera * cam, const AABB * bounding_box)
{
	float3 vCorner[8];
	int iTotalIn = 0;
	Plane planes[6];
	cam->frustum.GetPlanes(planes);

	bounding_box->GetCornerPoints(vCorner); // get the corners of the box into the vCorner array
											// test all 8 corners against the 6 sides
											// if all points are behind 1 specific plane, we are out
											// if we are in with all points, then we are fully in
	for (int p = 0; p < 6; ++p) {
		int iInCount = 8;
		int iPtIn = 1;
		for (int i = 0; i < 8; ++i) {
			// test this point against the planes
			if (!planes[p].AreOnSameSide(vCorner[i], cam->frustum.CenterPoint())) {
				iPtIn = 0;
				--iInCount;
			}
		}
		// were all the points outside of plane p?
		if (iInCount == 0)
			return false;
		// check if they were all on the right side of the plane
		iTotalIn += iPtIn;
	}
	// so if iTotalIn is 6, then all are inside the view
	if (iTotalIn == 6)
		return true;
	// we must be partly in then otherwise
	return true;
}

void ModuleGameObject::RecieveEvent(const Event & event)
{
	switch (event.type)
	{
	case EventType::Transform_Updated:
	{
		event.game_object.ptr->GetTransform()->CalculateGlobalTransforms();	

		UpdateTransforms(event.game_object.ptr);

		break;
	}
	case EventType::Update_Cam_Focus:
	{
		App->gameObj->Main_Cam->LookToObj(event.game_object.ptr, event.game_object.ptr->max_distance_point);

		break;
	}
	default:
		break;
	}
}

void ModuleGameObject::UpdateTransforms(GameObject * obj)
{
	if (obj->parent != nullptr)
		RecursiveUpdateParents(obj->parent);

	// For the camera components
	if (obj->GetComponent(CTypes::CT_Camera) != nullptr)
	{
		obj->GetComponent(CTypes::CT_Camera)->AsCamera()->TransformPos(obj->GetTransform()->global_pos);
		obj->GetComponent(CTypes::CT_Camera)->AsCamera()->TransformRot(obj->GetTransform()->global_rot);

		obj->GetComponent(CTypes::CT_Camera)->AsCamera()->SetbbFrustum();
	}

	for (int i = 0; i < obj->children.size(); i++)
		RecursiveUpdateChilds(obj->children[i]);

	//Calculating again BBs after the global transformations have been set.
	//First the childs, as the BBs of the parents are calculated from them.
	for (int i = 0; i < obj->children.size(); i++)
		RecursiveCalcBBsChilds(obj);

	CalculateBBs(obj);

	if (obj->parent != nullptr)
		RecursiveCalcBBsParents(obj);
}

void ModuleGameObject::RecursiveUpdateParents(GameObject * obj)
{
	if (obj->parent != nullptr)
		RecursiveUpdateParents(obj->parent);

	// For the camera components
	if (obj->GetComponent(CTypes::CT_Camera) != nullptr)
	{
		obj->GetComponent(CTypes::CT_Camera)->AsCamera()->TransformPos(obj->GetTransform()->global_pos);
		obj->GetComponent(CTypes::CT_Camera)->AsCamera()->TransformRot(obj->GetTransform()->global_rot);

		obj->GetComponent(CTypes::CT_Camera)->AsCamera()->SetbbFrustum();
	}
}

void ModuleGameObject::RecursiveUpdateChilds(GameObject * obj)
{
	for (int i = 0; i < obj->children.size(); i++)
		RecursiveUpdateChilds(obj->children[i]);

	// For the camera components
	if (obj->GetComponent(CTypes::CT_Camera) != nullptr)
	{
		obj->GetComponent(CTypes::CT_Camera)->AsCamera()->TransformPos(obj->GetTransform()->global_pos);
		obj->GetComponent(CTypes::CT_Camera)->AsCamera()->TransformRot(obj->GetTransform()->global_rot);

		obj->GetComponent(CTypes::CT_Camera)->AsCamera()->SetbbFrustum();
	}
}

void ModuleGameObject::CalculateBBs(GameObject * obj)
{
	obj->BoundingBox = new AABB(vec(INT_MAX, INT_MAX, INT_MAX), vec(INT_MIN, INT_MIN, INT_MIN));
	obj->max_distance_point = 0;

	std::vector<Component*> cmp_meshes;
	cmp_meshes = obj->GetComponents(CT_Mesh);

	if (obj->children.size() == 0)
	{
		if (cmp_meshes.size() > 0)
		{
			for (int i = 0; i < cmp_meshes.size(); i++)
			{
				math::AABB auxBB = *cmp_meshes[i]->AsMesh()->BoundingBox;
				auxBB.TransformAsAABB(cmp_meshes[i]->AsMesh()->parent->GetTransform()->global_transform);

				// Setting the BB min and max points

				if (obj->BoundingBox->maxPoint.x < auxBB.maxPoint.x)
					obj->BoundingBox->maxPoint.x = auxBB.maxPoint.x;
				if (obj->BoundingBox->minPoint.x > auxBB.minPoint.x)
					obj->BoundingBox->minPoint.x = auxBB.minPoint.x;
				if (obj->BoundingBox->maxPoint.y < auxBB.maxPoint.y)
					obj->BoundingBox->maxPoint.y = auxBB.maxPoint.y;
				if (obj->BoundingBox->minPoint.y > auxBB.minPoint.y)
					obj->BoundingBox->minPoint.y = auxBB.minPoint.y;
				if (obj->BoundingBox->maxPoint.z < auxBB.maxPoint.z)
					obj->BoundingBox->maxPoint.z = auxBB.maxPoint.z;
				if (obj->BoundingBox->minPoint.z > auxBB.minPoint.z)
					obj->BoundingBox->minPoint.z = auxBB.minPoint.z;
			}
		}
		else
		{
			obj->BoundingBox->maxPoint = obj->GetTransform()->global_pos + float3(1, 1, 1);
			obj->BoundingBox->minPoint = obj->GetTransform()->global_pos + float3(-1, -1, -1);
		}
	}
	else
	{
		for (int i = 0; i < obj->children.size(); i++)
		{
			if (obj->children[i]->BoundingBox != nullptr)
			{
				math::AABB* auxBB = obj->children[i]->BoundingBox;

				// Setting the BB min and max points with transforms

				if (obj->BoundingBox->maxPoint.x < auxBB->maxPoint.x)
					obj->BoundingBox->maxPoint.x = auxBB->maxPoint.x;
				if (obj->BoundingBox->minPoint.x > auxBB->minPoint.x)
					obj->BoundingBox->minPoint.x = auxBB->minPoint.x;
				if (obj->BoundingBox->maxPoint.y < auxBB->maxPoint.y)
					obj->BoundingBox->maxPoint.y = auxBB->maxPoint.y;
				if (obj->BoundingBox->minPoint.y > auxBB->minPoint.y)
					obj->BoundingBox->minPoint.y = auxBB->minPoint.y;
				if (obj->BoundingBox->maxPoint.z < auxBB->maxPoint.z)
					obj->BoundingBox->maxPoint.z = auxBB->maxPoint.z;
				if (obj->BoundingBox->minPoint.z > auxBB->minPoint.z)
					obj->BoundingBox->minPoint.z = auxBB->minPoint.z;
			}
		}
	}

	// Set maxPoint Value
	{
		if (abs(obj->BoundingBox->maxPoint.x) > obj->max_distance_point) { obj->max_distance_point = abs(obj->BoundingBox->maxPoint.x); }
		if (abs(obj->BoundingBox->maxPoint.y) > obj->max_distance_point) { obj->max_distance_point = abs(obj->BoundingBox->maxPoint.y); }
		if (abs(obj->BoundingBox->maxPoint.z) > obj->max_distance_point) { obj->max_distance_point = abs(obj->BoundingBox->maxPoint.z); }
		if (abs(obj->BoundingBox->minPoint.x) > obj->max_distance_point) { obj->max_distance_point = abs(obj->BoundingBox->minPoint.x); }
		if (abs(obj->BoundingBox->minPoint.y) > obj->max_distance_point) { obj->max_distance_point = abs(obj->BoundingBox->minPoint.y); }
		if (abs(obj->BoundingBox->minPoint.z) > obj->max_distance_point) { obj->max_distance_point = abs(obj->BoundingBox->minPoint.z); }
	}
}

void ModuleGameObject::RecursiveCalcBBsParents(GameObject * obj)
{
	CalculateBBs(obj);

	if (obj->parent != nullptr)
		RecursiveCalcBBsParents(obj->parent);
}

void ModuleGameObject::RecursiveCalcBBsChilds(GameObject * obj)
{
	for (int i = 0; i < obj->children.size(); i++)
		RecursiveCalcBBsChilds(obj->children[i]);

	CalculateBBs(obj);
}
