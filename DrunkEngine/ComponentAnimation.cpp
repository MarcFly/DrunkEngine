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
			if (phase != BlendPhase::End)
			{
				BlendFrom(r_anim->channels[i]);
			}
			else
				AnimateSkel(r_anim->channels[i]);
		}


	}
	if (phase != BlendPhase::End && playing)
		blend_timer += anims[prev_animation].tickrate / (1 / dt);

	if (playing)
		timer += anims[curr_animation].tickrate / (1 / dt);
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

	JSON_Array* anim_arr = json_object_get_array(comp, "animations");
	for (int i = 0; i < json_array_get_count(anim_arr); ++i)
	{
		VirtualAnimation push;

		JSON_Value* val = json_object_get_value(json_value_get_object(json_array_get_value(anim_arr, i)), "properties");
		JSON_Object* obj = json_value_get_object(val);

		push.start = json_object_get_number(obj, "start");
		push.end = json_object_get_number(obj, "end");
		push.tickrate = json_object_get_number(obj, "tickrate");
		push.blend_time = json_object_get_number(obj, "blend_time");

		anims.push_back(push);
	}

	if (!App->resources->InLibrary(UID))
		this->UID = App->resources->AddResource(name.c_str());
	if (App->resources->InLibrary(UID))
		App->importer->anim_i->LinkAnim(UID, this);

	if (r_anim != nullptr)
	{
		LinkMesh();
		App->scene->A3Animation = this;
	}
}

void ComponentAnimation::Save(JSON_Array* comps)
{
	JSON_Value* append = json_value_init_object();
	JSON_Object* curr = json_value_get_object(append);

	json_object_dotset_number(curr, "properties.type", type);

	json_object_dotset_string(curr, "properties.filename", App->resources->Library.at(UID)->file.c_str());
	json_object_dotset_number(curr, "properties.duration", duration);

	JSON_Value* set_array = json_value_init_array();
	JSON_Array* anim_arr = json_value_get_array(set_array);
	for (int i = 0; i < anims.size(); ++i)
	{
		JSON_Value* append_anim = json_value_init_object();
		JSON_Object* curr_anim = json_value_get_object(append_anim);

		json_object_dotset_number(curr_anim, "properties.start", anims[i].start);
		json_object_dotset_number(curr_anim, "properties.end", anims[i].end);
		json_object_dotset_number(curr_anim, "properties.tickrate", anims[i].tickrate);
		json_object_dotset_number(curr_anim, "properties.blend_time", anims[i].blend_time);

		json_array_append_value(anim_arr, append_anim);
	}

	json_object_dotset_value(curr, "properties.animations", set_array);
	//json_object_dotset_number(curr, "properties.tickrate", anims[0].tickrate);
	//Will have to save all the created animations

	json_array_append_value(comps, append);
}

void ComponentAnimation::BlendFrom(AnimChannel* curr_channel)
{
	float curr_anim_duration = abs(anims[curr_animation].end - anims[curr_animation].start);

	if (phase == BlendPhase::MidBlend)
	{
		if (blend_timer > anims[prev_animation].start + abs(anims[prev_animation].end - anims[prev_animation].start))
			blend_timer = anims[prev_animation].start;

		if (timer > anims[curr_animation].start + curr_anim_duration)
		{
			timer = anims[curr_animation].start;
			++blend_timer_loops;
		}
	}
	else
	{
		blend_timer = timer;
		timer = anims[curr_animation].start;
		phase = BlendPhase::MidBlend;
	}

	float total_blend_time = (timer - anims[curr_animation].start) + curr_anim_duration * blend_timer_loops;

	if (total_blend_time >= anims[curr_animation].blend_time)
	{
		phase = BlendPhase::End;
		blend_timer_loops = 0;
	}

	float3 Tprev_anim, Tnew_anim, step_pos;
	Quat Rprev_anim, Rnew_anim, step_rot;
	float3 Sprev_anim, Snew_anim, step_scale;

	float4x4 new_anim_matrix = curr_channel->GetMatrix(timer, duration, anims[curr_animation].tickrate);
	float4x4 prev_anim_matrix = curr_channel->GetMatrix(blend_timer, duration, anims[prev_animation].tickrate);

	prev_anim_matrix.Decompose(Tprev_anim, Rprev_anim, Sprev_anim);
	new_anim_matrix.Decompose(Tnew_anim, Rnew_anim, Snew_anim);

	float blend_percentage = total_blend_time / anims[curr_animation].blend_time;

	//Pos
	if (curr_channel->num_translation_keys > 1)
	{	
		step_pos = Tnew_anim * blend_percentage;
		step_pos += Tprev_anim * (1 - blend_percentage);
		curr_channel->curr_bone->transform.SetTransformPosition(step_pos.x, step_pos.y, step_pos.z);
	}
	//Rot (prob not working)
	if (curr_channel->num_rotation_keys > 1)
	{
		step_rot = Quat::Slerp(Rprev_anim, Rnew_anim, blend_percentage);

		curr_channel->curr_bone->transform.SetTransformRotation(step_rot * curr_channel->curr_bone->permanent_local_rot);
	}
	//Scale
	if (curr_channel->num_scaling_keys > 1)
	{
		step_scale = Sprev_anim * blend_percentage;
		step_scale += Snew_anim * (1 - blend_percentage);
		curr_channel->curr_bone->transform.SetTransformScale(step_scale.x, step_scale.y, step_scale.z);
	}
}

void ComponentAnimation::AnimateSkel(AnimChannel* curr_channel)
{
	if (timer > anims[curr_animation].start + abs(anims[curr_animation].end - anims[curr_animation].start))
		timer = anims[curr_animation].start;

	float4x4* curr_step = &curr_channel->curr_bone->last_anim_step;

	curr_channel->curr_bone->last_anim_step = curr_channel->GetMatrix(timer, duration, anims[curr_animation].tickrate);

	float3 step_pos;
	Quat step_rot;
	float3 step_scale;

	curr_step->Decompose(step_pos, step_rot, step_scale);

	// Do the animation calculations for skeleton here

	//Pos
	if (curr_channel->num_translation_keys > 1)
	{
		curr_channel->curr_bone->transform.SetTransformPosition(step_pos.x, step_pos.y, step_pos.z);
	}

	//Rot
	if (curr_channel->num_rotation_keys > 1)
	{
		curr_channel->curr_bone->transform.SetTransformRotation(step_rot * curr_channel->curr_bone->permanent_local_rot);
	}

	//Scale
	if (curr_channel->num_scaling_keys > 1)
	{
		curr_channel->curr_bone->transform.SetTransformScale(step_scale.x, step_scale.y, step_scale.z);
	}

	// We are sorry
	if (curr_animation == 2 && timer >= anims[curr_animation].end - 1)
	{
		curr_animation = 0;
		prev_animation = 2;
		phase = BlendPhase::Start;
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