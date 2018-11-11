#include "ObjectPropertiesWindow.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentCamera.h"


ObjectPropertiesWindow::ObjectPropertiesWindow() : Window("Object Properties")
{
	total_num_vertex = 0;
	total_num_faces = 0;
	check_info = false;
}

ObjectPropertiesWindow::~ObjectPropertiesWindow()
{
}

void ObjectPropertiesWindow::Draw()
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
					ComponentInspector(App->gameObj->active_objects[0]->components[i]);
				}

			}
		}

		else if (App->gameObj->active_objects.size() > 1) //objects.size() > 0)
		{
			ImGui::Text("+ 1 obj selected");
		}

		//if (ImGui::Button("Select")) {}
		//ImGui::SameLine();
		//if (ImGui::Button("Save")) {}
	}
	ImGui::End();
}

void ObjectPropertiesWindow::CheckMeshInfo()
{
	check_info = true;
	total_num_vertex = 0;
	total_num_faces = 0;
}

//----------------------------
// Component Inspectors
void ObjectPropertiesWindow::ComponentInspector(Component* component)
{
	switch (component->type)
	{
	case CT_Mesh: MeshInspector(component->AsMesh()); break;
	case CT_Material: MatInspector(component->AsMaterial()); break;
	case CT_Camera: CamInspector(component->AsCamera()); break;
	case CT_Transform: TransformInspector(component->AsTransform()); break;
	}
}

void ObjectPropertiesWindow::MeshInspector(ComponentMesh* mesh)
{
	if (ImGui::CollapsingHeader("Mesh"))
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

void ObjectPropertiesWindow::MatInspector(ComponentMaterial* mat)
{
	if (ImGui::CollapsingHeader("Material"))
	{

		for (int i = 0; mat != nullptr && i < mat->textures.size(); i++)
		{
			Texture* aux = mat->textures[i];
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
				std::vector<Component*> cmp_meshes;
				cmp_meshes = mat->parent->GetComponents(CT_Mesh);
				for (int j = 0; j < cmp_meshes.size(); j++)
					cmp_meshes[i]->AsMesh()->Material_Ind = i;
			}

			snprintf(str, 30, "%s%d%d", "Destroy this Texture ##", i, i);
			if (ImGui::Button(str))
				mat->DestroyTexture(i);
		}
	}
}

void ObjectPropertiesWindow::CamInspector(ComponentCamera* cam)
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Spacing();
		
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

void ObjectPropertiesWindow::TransformInspector(ComponentTransform* transform)
{
	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGui::Spacing();

		if (!App->gameObj->active_objects[0]->parent->is_static)
		{
			if (ImGui::Checkbox("Static", &transform->parent->is_static))
			{
				App->gameObj->active_objects[0]->RecursiveSetStatic(transform->parent, transform->parent->is_static);

				if (transform->parent->is_static)
					App->gameObj->SetToStaticObjects(transform->parent);
				else
					App->gameObj->DeleteFromStaticObjects(transform->parent);
			}
		}
		else
			ImGui::Text("Parent is static, you cannot modify this object");

		ImGui::Spacing();

		ImGui::Text("Use CTRL + click to input value");

		ImGui::Spacing();

		//Pos
		float pos[3] = { transform->position.x, transform->position.y, transform->position.z };
		if (!transform->parent->is_static && ImGui::DragFloat3("Position", pos, 0.1f))
			transform->SetTransformPosition(pos[0], pos[1], pos[2]);

		//Scale
		float scale[3] = { transform->scale.x, transform->scale.y, transform->scale.z };
		if (!transform->parent->is_static && ImGui::DragFloat3("Scale", scale, 0.1f))
			transform->SetTransformScale(scale[0], scale[1], scale[2]);

		//Rot
		float rot[3] = { transform->rotate_euler.x, transform->rotate_euler.y, transform->rotate_euler.z };
		if (!transform->parent->is_static && ImGui::DragFloat3("Rotation", rot, 0.2f))
			transform->SetTransformRotation((float3)rot);

		ImGui::Spacing();
	}
}