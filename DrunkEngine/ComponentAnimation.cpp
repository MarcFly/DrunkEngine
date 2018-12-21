#include "ComponentAnimation.h"
#include "Application.h"
#include "AnimationImport.h"
#include "ComponentSkeleton.h"
#include "ResourceSkeleton.h"

ComponentAnimation::ComponentAnimation()
{
	SetBaseVals();
}
ComponentAnimation::ComponentAnimation(GameObject* par)
{
	SetBaseVals();

	parent = par;
}

ComponentAnimation::~ComponentAnimation()
{
}

void ComponentAnimation::Start()
{
}

void ComponentAnimation::Update(const float dt)
{
	for (int i = 0; i < r_anim->channels.size(); i++)
	{
		if (r_anim->channels[i]->curr_bone != nullptr)
		{
			float4x4* curr_step = &r_anim->channels[i]->curr_bone->last_anim_step;
			if (!curr_step->IsIdentity())
			{

			}
			r_anim->channels[i]->curr_bone->last_anim_step = r_anim->channels[i]->GetMatrix(timer, duration, tickrate);
			
			//float4x4 base_t = float4x4::FromTRS(r_anim->channels[i]->curr_bone->permanent_local_pos, r_anim->channels[i]->curr_bone->permanent_local_rot, r_anim->channels[i]->curr_bone->permanent_local_scale);
			
			float3 step_pos;
			Quat step_rot;
			float3 step_scale;

			curr_step->Decompose(step_pos, step_rot, step_scale);

			// Do the animation calculations for skeleton here
			
			//Pos
			if (r_anim->channels[i]->num_translation_keys > 1)
			{
				r_anim->channels[i]->curr_bone->transform.SetTransformPosition(r_anim->channels[i]->curr_bone->permanent_local_pos.x + step_pos.x, r_anim->channels[i]->curr_bone->permanent_local_pos.y + step_pos.y, r_anim->channels[i]->curr_bone->permanent_local_pos.z + step_pos.z);
			}

			//Rot
			if (r_anim->channels[i]->num_rotation_keys > 1)
			{
				r_anim->channels[i]->curr_bone->transform.SetTransformRotation(r_anim->channels[i]->curr_bone->permanent_local_rot * step_rot);
			}

			//Scale
			if (r_anim->channels[i]->num_scaling_keys > 1)
			{
				r_anim->channels[i]->curr_bone->transform.SetTransformScale(r_anim->channels[i]->curr_bone->permanent_local_scale.x + step_scale.x - 1, r_anim->channels[i]->curr_bone->permanent_local_scale.y + step_scale.y - 1, r_anim->channels[i]->curr_bone->permanent_local_scale.z + step_scale.z - 1);
			}	
			

			//base_t = base_t * curr_step;
			//
			//r_anim->channels[i]->curr_bone->transform.SetFromMatrix(&base_t);
		}


	}

	timer += tickrate / (1 / dt);

	if (timer > duration)
		timer = 0;
}

void ComponentAnimation::Draw()
{
}

void ComponentAnimation::CleanUp()
{
}


void ComponentAnimation::Load(const JSON_Object* comp)
{
	this->name = json_object_get_string(comp, "filename");
	this->UID = DGUID(name.c_str());


	if (!App->resources->InLibrary(UID))
		this->UID = App->resources->AddResource(name.c_str());
	if (App->resources->InLibrary(UID))
		App->importer->anim_i->LinkAnim(UID, this);

	LinkMesh();
}

void ComponentAnimation::Save(JSON_Array* comps)
{
	JSON_Value* append = json_value_init_object();
	JSON_Object* curr = json_value_get_object(append);

	json_object_dotset_number(curr, "properties.type", type);

	json_object_dotset_string(curr, "properties.filename", App->resources->Library.at(UID)->file.c_str());
	json_object_dotset_number(curr, "properties.duration", duration);
	json_object_dotset_number(curr, "properties.tickrate", tickrate);


	json_array_append_value(comps, append);
}

void ComponentAnimation::LinkMesh()
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

	if (c_skel == nullptr)
		for (int i = 0; i < parent->components.size(); i++)
		{
			if (parent->components[i]->type == CT_Skeleton)
			{
				c_skel = parent->components[i]->AsSkeleton();
				break;
			}
		}

	for (int i = 0; i < r_anim->channels.size(); i++)
	{
		r_anim->channels[i]->curr_bone = c_skel->r_skel->FindBone(r_anim->channels[i]->bone_name);
	}
}