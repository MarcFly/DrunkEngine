#include "ComponentMaterial.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "Inspector.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ModuleImport.h"
#include "MaterialImport.h"
#include "ResourceTexture.h"
#include "ResourceMaterial.h"

ComponentMaterial::ComponentMaterial(GameObject* par)
{
	SetBaseVals();

	parent = par;
}

void ComponentMaterial::DestroyTexture(const int& tex_ind)
{
	glDeleteTextures(1, &r_mat->textures[tex_ind]->id_tex);

	r_mat->textures[tex_ind] = nullptr;

	PopTexture(tex_ind);
}

void ComponentMaterial::PopTexture(const int& tex_index)
{

	for (int i = tex_index + 1; i < r_mat->textures.size(); i++)
		r_mat->textures[i - 1] = r_mat->textures[i];

	r_mat->textures.pop_back();
}

void ComponentMaterial::CleanUp()
{

	/*for (int i = 0; i < r_mat->textures.size(); i++)
		DestroyTexture(i);*/


	parent = nullptr;
}

void ComponentMaterial::Load(JSON_Object* comp)
{
	this->name = json_object_get_string(comp, "filename");
	this->UID = DGUID(GetMD5ID(name).c_str());

	if (!App->resources->InLibrary(UID))
		this->UID = App->resources->AddResource(name.c_str());
	if (App->resources->InLibrary(UID))
		App->importer->mat_i->LinkMat(UID, this);
	else
		App->ui->console_win->AddLog("Not in library!");
}

void ComponentMaterial::Save(JSON_Array* comps)
{
	JSON_Value* append = json_value_init_object();
	JSON_Object*  curr = json_value_get_object(append);

	json_object_dotset_number(curr, "properties.type", type);

	json_object_dotset_string(curr, "properties.mat_name", name.c_str());

	json_array_append_value(comps, append);

	App->importer->mat_i->ExportMat(this);
}