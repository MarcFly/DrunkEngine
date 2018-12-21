#include "InspectorWindow.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentCamera.h"
#include "ComponentBillboard.h"
#include "ComponentSkeleton.h"
#include "ComponentAnimation.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "ResourceAnimation.h"
#include "KdTreeWindow.h"
#include "SkeletonInspector.h"
#include "AnimationInspector.h"

Inspector::Inspector() : Window("Inspector")
{
	total_num_vertex = 0;
	total_num_faces = 0;
	check_info = false;

	skel_ins = new SkeletonInspectorWin();
	anim_ins = new AnimationInspectorWin();
}

Inspector::~Inspector()
{

	delete skel_ins;
	skel_ins = nullptr;

	delete anim_ins;
	anim_ins = nullptr;

}

void Inspector::Draw()
{
	ImGui::Begin(GetName().c_str(), &active);
	{
		if (App->gameObj->active_objects.size() == 1) //objects.size() > 0)
		{
			ImGui::Text("%s", App->gameObj->active_objects[0]->name.c_str());

			if (App->gameObj->active_objects[0]->parent != nullptr)
			{
				for (int i = 0; i < App->gameObj->active_objects[0]->components.size(); i++)
				{
					ComponentInspector(App->gameObj->active_objects[0]->components[i], i);
					ImGui::Separator();
				}
			}
		}

		else if (App->gameObj->active_objects.size() > 1)
		{
			ImGui::Text("+ 1 obj selected");
		}
	}
	ImGui::End();
}

void Inspector::CheckMeshInfo()
{
	check_info = true;
	total_num_vertex = 0;
	total_num_faces = 0;
}

//----------------------------
// Component Inspectors
void Inspector::ComponentInspector(Component* component, const int& cmpt_id)
{
	std::string cb_active_name = "##active_" + component->name;
	ImGui::Checkbox(cb_active_name.c_str(), &component->active);
	ImGui::SameLine();
	switch (component->type)
	{
	case CT_Mesh: MeshInspector(component->AsMesh(), cmpt_id); break;
	case CT_Material: MatInspector(component->AsMaterial(), cmpt_id); break;
	case CT_Camera: CamInspector(component->AsCamera(), cmpt_id); break;
	case CT_Transform: TransformInspector(component->AsTransform(), cmpt_id); break;
	case CT_Billboard: BillboardInspector(component->AsBillboard(), cmpt_id); break;
	case CT_Skeleton: SkeletonInspector(component->AsSkeleton(), cmpt_id); break;
	case CT_Animation: AnimationInspector(component->AsAnimation(), cmpt_id); break;
	}
}

void Inspector::MeshInspector(ComponentMesh* mesh, const int& cmpt_id)
{
	std::string HeaderID = "Mesh: " + mesh->name + "##" + std::to_string(cmpt_id);
	if (ImGui::CollapsingHeader(HeaderID.c_str()))
	{
		if (check_info)
		{
			std::vector<uint> props = mesh->parent->GetMeshProps();

			total_num_vertex = props[0];
			total_num_faces = props[1];
			check_info = false;
		}
		ImGui::Text("Total Num. Vertices: %d", total_num_vertex);
		ImGui::Text("Total Num. Faces: %d", total_num_faces);
	}
}

void Inspector::MatInspector(ComponentMaterial* mat, const int& cmpt_id)
{
	std::string HeaderID = "Matrial: " + mat->name + "##" + std::to_string(cmpt_id);
	if (ImGui::CollapsingHeader(HeaderID.c_str()))
	{

		for (int i = 0; mat != nullptr && i < mat->r_mat->textures.size(); i++)
		{
			ResourceTexture* aux = mat->textures[i];
			ImGui::Separator();

			if (check_info)
				tex_name = aux->filename.c_str();

			ImGui::Image(ImTextureID(aux->id_tex), show_size);

			if (strrchr(tex_name.c_str(), '\\') != nullptr)
				tex_name = tex_name.substr(tex_name.find_last_of("\\/") + 1);

			ImGui::TextWrapped("Texture File: %s", tex_name.c_str());

			ImGui::Text("Size: %d x %d", aux->width, aux->height);

			char str[30];
			snprintf(str, 30, "%s%d", "Use this Texture ##", i);

			if (ImGui::Button(str))
			{
				/*std::vector<Component*> cmp_meshes;
				cmp_meshes = mat->parent->GetComponents(CT_Mesh);
				for (int j = 0; j < cmp_meshes.size(); j++)
					cmp_meshes[i]->AsMesh()->Material_Ind = i;*/
			}

			snprintf(str, 30, "%s%d%d", "Destroy this Texture ##", i, i);
			if (ImGui::Button(str))
			{
			}
				//mat->DestroyTexture(i);

			ImGui::Checkbox("AlphaTest", &mat->AlphaTest);
			ImGui::SliderFloat("AlphaVal", &mat->AlphaVal, 0, 1);

		}
	}
}

void Inspector::CamInspector(ComponentCamera* cam, const int& cmpt_id)
{
	std::string HeaderID = "Camera: " + cam->name + "##" + std::to_string(cmpt_id);
	if (ImGui::CollapsingHeader(HeaderID.c_str()))
	{
		ImGui::Spacing();
		
		if (ImGui::Checkbox("Main Cam", &main_cam))
		{
			if (main_cam)
				App->gameObj->SetCameraRender(cam);
			else
				App->gameObj->SetCameraRender(App->gameObj->Main_Cam);
		}
		//float aux_fov = RadToDeg(cam->original_v_fov);  If scale is compatible whith camera
		float aux_fov = RadToDeg(cam->frustum.verticalFov);
		if (ImGui::DragFloat("FOV", &aux_fov, 1.0f, 0.0, 179))
		{
			//cam->original_v_fov = DegToRad(aux_fov);	If scale is compatible whith camera
			cam->frustum.verticalFov = DegToRad(aux_fov);
			cam->SetbbFrustum();
			cam->SetAspectRatio();
		}

		if (ImGui::SliderFloat("NearPlane", &cam->frustum.nearPlaneDistance, 0.5f, 200.0f))
		{
			cam->frustum.SetViewPlaneDistances(cam->frustum.nearPlaneDistance, cam ->frustum.farPlaneDistance);
			cam->SetbbFrustum();
			cam->SetAspectRatio();
		}

		if (ImGui::SliderFloat("FarPlane", &cam->frustum.farPlaneDistance, 1.f, 1000.0f))
		{
			cam->frustum.SetViewPlaneDistances(cam->frustum.nearPlaneDistance, cam->frustum.farPlaneDistance);
			cam->SetbbFrustum();
			cam->SetAspectRatio();
		}
	}
}

void Inspector::TransformInspector(ComponentTransform* transform, const int& cmpt_id)
{
	std::string HeaderID = "Transform: " + transform->name + "##" + std::to_string(cmpt_id);
	if (ImGui::CollapsingHeader(HeaderID.c_str()))
	{
		ImGui::Spacing();

		if (!App->gameObj->active_objects[0]->parent->is_static)
		{
			if (ImGui::Checkbox("Static", &transform->parent->is_static))
			{
				App->gameObj->active_objects[0]->RecursiveSetStatic(transform->parent, transform->parent->is_static);

				if (transform->parent->is_static)
				{
					App->gameObj->SetToStaticObjects(transform->parent);
					App->ui->kdtree_win->CreateKDTree();
				}
				else
				{
					App->gameObj->DeleteFromStaticObjects(transform->parent);
					App->ui->kdtree_win->CreateKDTree();
				}
			}
		}
		else
			ImGui::Text("Parent is static, you cannot modify this object");

		ImGui::Spacing();

		ImGui::Text("Use CTRL + click to input value");

		ImGui::Spacing();

		if (ImGui::RadioButton("World", radio_world))
		{
			radio_world = true;
			radio_local = false;
			App->gameObj->mCurrentGizmoMode = ImGuizmo::WORLD;
		}

		ImGui::SameLine();

		if (ImGui::RadioButton("Local", radio_local))
		{
			radio_world = false;
			radio_local = true;
			App->gameObj->mCurrentGizmoMode = ImGuizmo::LOCAL;
		}

		ImGui::Spacing();

		if (App->gameObj->mCurrentGizmoMode == ImGuizmo::LOCAL)
		{
			//Pos
			float pos[3] = { transform->position.x, transform->position.y, transform->position.z };
			if (!transform->parent->is_static && ImGui::DragFloat3("Position", pos, 0.2f))
			{
				transform->SetTransformPosition(pos[0], pos[1], pos[2]);

				Event ev(EventType::Transform_Updated, Event::UnionUsed::UseGameObject);
				ev.game_object.ptr = transform->parent;
				App->eventSys->BroadcastEvent(ev);
			}

			//Rot
			float rot[3] = { transform->rotate_euler.x, transform->rotate_euler.y, transform->rotate_euler.z };
			if (!transform->parent->is_static && ImGui::DragFloat3("Rotation", rot, 0.5f))
			{
				transform->SetTransformRotation((float3)rot);

				Event ev(EventType::Transform_Updated, Event::UnionUsed::UseGameObject);
				ev.game_object.ptr = transform->parent;
				App->eventSys->BroadcastEvent(ev);
			}
		}
		else if (App->gameObj->mCurrentGizmoMode == ImGuizmo::WORLD)
		{	
			float3 pos_vec;
			Quat rot_quat;
			float3 aux;

			//transform->aux_world_pos.Decompose(pos_vec, rot_quat, aux);
			pos_vec = transform->aux_world_pos.Col3(3);
			transform->world_rot.Decompose(aux, rot_quat, aux);

			//Pos
			float pos[3] = { pos_vec.x, pos_vec.y, pos_vec.z };
			if (!transform->parent->is_static && ImGui::DragFloat3("Position", pos, 0.2f))
			{
				transform->SetWorldPos(float4x4::FromTRS(float3(pos[0] - pos_vec.x, pos[1] - pos_vec.y, pos[2] - pos_vec.z), Quat::identity, float3::one));

				Event ev(EventType::Transform_Updated, Event::UnionUsed::UseGameObject);
				ev.game_object.ptr = transform->parent;
				App->eventSys->BroadcastEvent(ev);
			}

			//Rot
			float3 vec_rot = RadToDeg(rot_quat.ToEulerXYZ());
			float rot[3] = { vec_rot.x, vec_rot.y, vec_rot.z };
			if (!transform->parent->is_static && ImGui::DragFloat3("Rotation", rot, 0.5f))
			{
				Quat rot_quat = Quat::FromEulerXYZ(DegToRad(rot[0] - vec_rot.x), DegToRad(rot[1] - vec_rot.y), DegToRad(rot[2] - vec_rot.z));
				transform->SetWorldRot(rot_quat);

				Event ev(EventType::Transform_Updated, Event::UnionUsed::UseGameObject);
				ev.game_object.ptr = transform->parent;
				App->eventSys->BroadcastEvent(ev);
			}
		}

		//Scale
		float scale[3] = { transform->scale.x, transform->scale.y, transform->scale.z };
		if (!transform->parent->is_static && ImGui::DragFloat3("Scale", scale, 0.2f))
		{
			transform->SetTransformScale(scale[0], scale[1], scale[2]);

			Event ev(EventType::Transform_Updated, Event::UnionUsed::UseGameObject);
			ev.game_object.ptr = transform->parent;
			App->eventSys->BroadcastEvent(ev);
		}
		ImGui::Spacing();
	}
}

void Inspector::BillboardInspector(ComponentBillboard* billboard, const int& cmpt_id)
{

}

void Inspector::SkeletonInspector(ComponentSkeleton* skel, const int& cmpt_id)
{
	if (ImGui::Button("Open Skeleton Inspector"))
		skel_ins->active = true;

	skel_ins->base_skel = skel;
	skel_ins->Draw();
}

void Inspector::AnimationInspector(ComponentAnimation* anim, const int& cmpt_id)
{
	std::string HeaderID = "Animation: " + anim->name + "##" + std::to_string(cmpt_id);
	if (ImGui::CollapsingHeader(HeaderID.c_str()))
	{
		ImGui::Text("Duration in Frames: ");
		ImGui::SameLine();
		if (anim->duration < 0)
			anim->duration = 0;
		ImGui::DragInt("##Duration", &anim->duration, 0, 9999999);

		ImGui::Text("Framerate: ");
		ImGui::SameLine();
		if (anim->tickrate < 1)
			anim->tickrate = 1;
		ImGui::DragInt("##Framerate", &anim->tickrate, 1, 9999999);

		if (ImGui::Button("Open Animation Inspector"))
			anim_ins->active = true;

		anim_ins->anim = anim;
		anim_ins->Draw();
	}
}