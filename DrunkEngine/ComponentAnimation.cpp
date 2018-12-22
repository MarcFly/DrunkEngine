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

	if(playing)
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
	}

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
	if (timer > anims[curr_animation].blend_time)
	{
		timer = anims[curr_animation].start;
		blending = false;
	}

	float3 TA, TB, step_pos;
	Quat RA, RB, step_rot;
	float3 SA, SB, step_scale;
	float4x4 start_step = curr_channel->GetFirstFrame(duration, anims[curr_animation].tickrate);
	
	curr_channel->curr_bone->last_anim_step.Decompose(TA, RA, SA);
	start_step.Decompose(TB, RB, SB);

	step_pos = TB * (timer / anims[curr_animation].blend_time);
	step_pos += TA * (timer / anims[curr_animation].blend_time);

	float3 rot_test = RadToDeg(RB.ToEulerXYZ());
	float3 ret_rot;
	ret_rot = rot_test * anims[curr_animation].blend_time;
	ret_rot += rot_test * (1 - anims[curr_animation].blend_time);
	step_rot = Quat::FromEulerXYZ(DegToRad(ret_rot.x), DegToRad(ret_rot.y), DegToRad(ret_rot.z));

	step_scale = SB * (timer / anims[curr_animation].blend_time);
	step_scale += SA * (timer / anims[curr_animation].blend_time);

	curr_channel->curr_bone->transform.SetTransformPosition(curr_channel->curr_bone->permanent_local_pos.x + step_pos.x, curr_channel->curr_bone->permanent_local_pos.y + step_pos.y, curr_channel->curr_bone->permanent_local_pos.z + step_pos.z);
	curr_channel->curr_bone->transform.SetTransformPosition(curr_channel->curr_bone->permanent_local_pos.x + step_pos.x, curr_channel->curr_bone->permanent_local_pos.y + step_pos.y, curr_channel->curr_bone->permanent_local_pos.z + step_pos.z);
	curr_channel->curr_bone->transform.SetTransformScale(curr_channel->curr_bone->permanent_local_scale.x + step_scale.x - 1, curr_channel->curr_bone->permanent_local_scale.y + step_scale.y - 1, curr_channel->curr_bone->permanent_local_scale.z + step_scale.z - 1);

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