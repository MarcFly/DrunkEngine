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
			if (blending == true)
				BlendFrom(r_anim->channels[i]);
			else
				AnimateSkel(r_anim->channels[i]);
		}


	}

	timer += tickrate / (1 / dt);
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

void ComponentAnimation::BlendFrom(AnimChannel* curr_channel)
{
	if (timer > anim_blend)
	{
		timer = 0;
		blending = false;
	}

	float3 TA, TB, step_pos;
	Quat RA, RB, step_rot;
	float3 SA, SB, step_scale;
	float4x4 start_step = curr_channel->GetFirstFrame(duration, tickrate);
	
	curr_channel->curr_bone->last_anim_step.Decompose(TA, RA, SA);
	start_step.Decompose(TB, RB, SB);

	step_pos = TB * (timer / anim_blend);
	step_pos += TA * (timer / anim_blend);

	float3 rot_test = RadToDeg(RB.ToEulerXYZ());
	float3 ret_rot;
	ret_rot = rot_test * anim_blend;
	ret_rot += rot_test * (1 - anim_blend);
	step_rot = Quat::FromEulerXYZ(DegToRad(ret_rot.x), DegToRad(ret_rot.y), DegToRad(ret_rot.z));

	step_scale = SB * (timer / anim_blend);
	step_scale += SA * (timer / anim_blend);

	curr_channel->curr_bone->transform.SetTransformPosition(curr_channel->curr_bone->permanent_local_pos.x + step_pos.x, curr_channel->curr_bone->permanent_local_pos.y + step_pos.y, curr_channel->curr_bone->permanent_local_pos.z + step_pos.z);
	curr_channel->curr_bone->transform.SetTransformPosition(curr_channel->curr_bone->permanent_local_pos.x + step_pos.x, curr_channel->curr_bone->permanent_local_pos.y + step_pos.y, curr_channel->curr_bone->permanent_local_pos.z + step_pos.z);
	curr_channel->curr_bone->transform.SetTransformScale(curr_channel->curr_bone->permanent_local_scale.x + step_scale.x - 1, curr_channel->curr_bone->permanent_local_scale.y + step_scale.y - 1, curr_channel->curr_bone->permanent_local_scale.z + step_scale.z - 1);

}

void ComponentAnimation::AnimateSkel(AnimChannel* curr_channel)
{
	if (timer > duration)
		timer = 0;

	float4x4* curr_step = &curr_channel->curr_bone->last_anim_step;

	curr_channel->curr_bone->last_anim_step = curr_channel->GetMatrix(timer, duration, tickrate);

	float3 step_pos;
	Quat step_rot;
	float3 step_scale;

	curr_step->Decompose(step_pos, step_rot, step_scale);

	// Do the animation calculations for skeleton here

	//Pos
	if (curr_channel->num_translation_keys > 1)
	{
		curr_channel->curr_bone->transform.SetTransformPosition(curr_channel->curr_bone->permanent_local_pos.x + step_pos.x, curr_channel->curr_bone->permanent_local_pos.y + step_pos.y, curr_channel->curr_bone->permanent_local_pos.z + step_pos.z);
	}

	//Rot
	if (curr_channel->num_rotation_keys > 1)
	{
		curr_channel->curr_bone->transform.SetTransformRotation(curr_channel->curr_bone->permanent_local_rot * step_rot);
	}

	//Scale
	if (curr_channel->num_scaling_keys > 1)
	{
		curr_channel->curr_bone->transform.SetTransformScale(curr_channel->curr_bone->permanent_local_scale.x + step_scale.x - 1, curr_channel->curr_bone->permanent_local_scale.y + step_scale.y - 1, curr_channel->curr_bone->permanent_local_scale.z + step_scale.z - 1);
	}
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