#include "ComponentSkeleton.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "SceneViewerWindow.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ResourceSkeleton.h"
#include "FileHelpers.h"
#include "ResourceMesh.h"

ComponentSkeleton::ComponentSkeleton()
{
	SetBaseVals();
}

ComponentSkeleton::ComponentSkeleton(GameObject* par)
{
	SetBaseVals();

	parent = par;
};

void ComponentSkeleton::Update(const float dt)
{
	if (check_vecs < 2)
	{
		initial_pos = parent->GetTransform()->global_transform.Col3(3);
		initial_rot = parent->GetTransform()->GetRotFromMat(parent->GetTransform()->global_transform);
		initial_scale = parent->GetTransform()->global_transform.GetScale();

		check_vecs++;
	}

	UpdateTransform();


}

void ComponentSkeleton::Draw()
{
	glBegin(GL_LINES);

	glColor3f(1.0f, 0.1f, 0.0f);
	for (int i = 0; i < r_skel->bones.size(); i++)
		DrawToChildren(r_skel->bones[i]);

	glColor3f(0, 1, 0);

	DrawDeformedMesh();

	glEnd();
}

void ComponentSkeleton::DrawDeformedMesh()
{
	c_mesh->deformable_mesh->SetValsFromMesh(c_mesh->r_mesh);
	
	DeformMesh(r_skel->bones, c_mesh->deformable_mesh);
}

void ComponentSkeleton::DeformMesh(std::vector<Bone*>& bones, ResourceMesh * deformable_mesh)
{
	for (int i = 0; i < bones.size(); ++i)
	{

		float4x4 b_trans = bones[i]->transform.global_transform;

		for (int j = 0; j < bones[i]->weights.size(); ++j)
		{
			GLfloat* curr_vertex = &deformable_mesh->vertex[deformable_mesh->index[bones[i]->weights[j]->VertexID]/3];
			float3 movement(curr_vertex[0], curr_vertex[1], curr_vertex[2]);
			movement = b_trans.TransformPos(movement);
			curr_vertex[0] += movement.x * bones[i]->weights[j]->w;
			curr_vertex[1] += movement.y * bones[i]->weights[j]->w;
			curr_vertex[2] += movement.z * bones[i]->weights[j]->w;

			GLfloat* curr_normal = &deformable_mesh->vert_normals[deformable_mesh->index[bones[i]->weights[j]->VertexID]/3];
			movement = float3(curr_normal[0], curr_normal[1], curr_normal[2]);
			movement = b_trans.TransformPos(movement);
			curr_normal[0] += movement.x * bones[i]->weights[j]->w;
			curr_normal[1] += movement.y * bones[i]->weights[j]->w;
			curr_normal[2] += movement.z * bones[i]->weights[j]->w;
		}

		for (int j = 0; j < bones[i]->children.size(); ++j)
			DeformMesh(bones[i]->children, deformable_mesh);
	}
}

void ComponentSkeleton::DrawToChildren(Bone* bone)
{
	for (int i = 0; i < bone->children.size(); i++)
	{
		//if (bone->parent != nullptr)
		//{
			glVertex3f(bone->transform.global_pos.x, bone->transform.global_pos.y, bone->transform.global_pos.z);
			glVertex3f(bone->children[i]->transform.global_pos.x, bone->children[i]->transform.global_pos.y, bone->children[i]->transform.global_pos.z);
		//}

		DrawToChildren(bone->children[i]);
	}
}

void ComponentSkeleton::CleanUp()
{
	App->resources->Unused(UID);
	r_skel = nullptr;
	c_mesh = nullptr;

	//delete deformable_mesh;
	//deformable_mesh = nullptr;
}

void ComponentSkeleton::Load(const JSON_Object* comp)
{
	this->name = json_object_get_string(comp, "filename");
	this->UID = DGUID(name.c_str());


	if (!App->resources->InLibrary(UID))
		this->UID = App->resources->AddResource(name.c_str());
	if (App->resources->InLibrary(UID))
		App->importer->skel_i->LinkSkeleton(UID, this);

	CreateDeformableMesh();
}

void ComponentSkeleton::Save(JSON_Array* comps)
{
	JSON_Value* append = json_value_init_object();
	JSON_Object* curr = json_value_get_object(append);

	json_object_dotset_number(curr, "properties.type", type);

	json_object_dotset_string(curr, "properties.filename", App->resources->Library.at(UID)->file.c_str());

	json_array_append_value(comps, append);

}

void ComponentSkeleton::UpdateTransform()
{
	ComponentTransform* root_transform = &r_skel->bones[0]->transform;
	ComponentTransform* parent_transform = parent->GetTransform();

	//1
	/*{
		root_transform->global_transform = this->parent->GetTransform()->global_transform;

		//Global values
		//root_transform->global_pos = parent->getObjectCenter();
		root_transform->global_pos = root_transform->global_transform.Col3(3);

		root_transform->global_rot = root_transform->GetRotFromMat(root_transform->global_transform);

		//root_transform->global_transform.Decompose(root_transform->global_pos, root_transform->global_rot, root_transform->global_scale);

		//root_transform->global_pos -= initial_pos;
		root_transform->global_scale = float3(root_transform->global_transform[0][0] / initial_scale.x, root_transform->global_transform[1][1] / initial_scale.y, root_transform->global_transform[2][2] / initial_scale.z);

		//Position
		root_transform->global_transform.SetCol3(3, root_transform->global_pos - initial_pos);

		//Rotation
		root_transform->global_rot.x = root_transform->global_rot.x - initial_rot.x;
		root_transform->global_rot.y = root_transform->global_rot.y - initial_rot.y;
		root_transform->global_rot.z = root_transform->global_rot.z - initial_rot.z;
		root_transform->global_rot.w = root_transform->global_rot.w - initial_rot.w;

		//root_transform->global_transform = float4x4::FromTRS(root_transform->global_pos, root_transform->global_rot, root_transform->global_scale);

		//Scale
		root_transform->global_transform[0][0] = root_transform->global_scale.x;
		root_transform->global_transform[1][1] = root_transform->global_scale.y;
		root_transform->global_transform[2][2] = root_transform->global_scale.z;

		//root_transform->global_transform.Scale(root_transform->global_scale, parent->GetTransform()->global_pos); //not working
	}*/

	//2
	{


		root_transform->global_pos = parent_transform->global_pos - initial_pos;

		root_transform->global_rot.x = parent_transform->global_rot.x;
		root_transform->global_rot.y = parent_transform->global_rot.y;
		root_transform->global_rot.z = parent_transform->global_rot.z;
		root_transform->global_rot.w = parent_transform->global_rot.w;

		root_transform->global_scale = float3(parent->GetTransform()->global_scale.x / initial_scale.x, parent->GetTransform()->global_scale.y / initial_scale.y, parent->GetTransform()->global_scale.z / initial_scale.z);
		//root_transform->global_scale = float3(1, 1, 1);

		root_transform->global_transform = float4x4::FromTRS(root_transform->global_pos, root_transform->global_rot, root_transform->global_scale);

	}


	r_skel->CalculateSkeletonTransforms();
}

bool CkeckSkeletonValidity()
{
	return true;
}

/*
void ComponentSkeleton::LinkMesh()
{
		
}*/

void ComponentSkeleton::CreateDeformableMesh()
{
	if (c_mesh == nullptr)
		for (int i = 0; i < parent->components.size(); i++)
		{
			if (parent->components[i]->type == CT_Mesh)
			{
				c_mesh = parent->components[i]->AsMesh();
				break;
			}
		}

	c_mesh->deformable_mesh = new ResourceMesh(c_mesh->r_mesh);


}
