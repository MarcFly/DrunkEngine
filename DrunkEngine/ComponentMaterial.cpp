#include "ComponentMaterial.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "SceneViewerWindow.h"
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

void ComponentMaterial::PopTexture(const int& tex_index)
{
	if (textures.size() > 0)
	{
		App->resources->Unused(r_mat->textures[tex_index]);
		textures[tex_index] = nullptr;

		for (int i = tex_index + 1; i < r_mat->textures.size(); i++)
		{
			r_mat->textures[i - 1] = r_mat->textures[i];
			textures[i - 1] = textures[i];
		}

		r_mat->textures.pop_back();
	}
}

void ComponentMaterial::CleanUp()
{
	for (int i = 0; i < parent->components.size(); i++)
	{
		if (parent->components[i] != nullptr)
		{
			ComponentMesh* mesh = parent->components[i]->AsMesh();
			if (mesh != nullptr)
				if (mesh->c_mat == this)
				{
					mesh->c_mat = nullptr;
					mesh->Material_Ind.SetInvalid();
				}
		}
	}

	for (int i = 0; i < r_mat->textures.size(); i++)
		PopTexture(i);

	App->resources->Unused(UID);
	r_mat = nullptr;

	parent = nullptr;
}

void ComponentMaterial::Load(const JSON_Object* comp)
{
	this->name = json_object_get_string(comp, "filename");
	this->UID = DGUID(name.c_str());

	if (!App->resources->InLibrary(UID))
		this->UID = App->resources->AddResource(name.c_str());
	if (App->resources->InLibrary(UID))
		App->importer->mat_i->LinkMat(UID, this);
	else
		App->ui->console_win->AddLog("Not in library!");

	for (int i = 0; i < r_mat->textures.size(); i++)
		this->textures.push_back(App->importer->mat_i->LinkTexture(r_mat->textures[i]));

}

void ComponentMaterial::Save(JSON_Array* comps)
{
	JSON_Value* append = json_value_init_object();
	JSON_Object*  curr = json_value_get_object(append);

	json_object_dotset_number(curr, "properties.type", type);

	json_object_dotset_string(curr, "properties.filename", name.c_str());

	json_array_append_value(comps, append);
}