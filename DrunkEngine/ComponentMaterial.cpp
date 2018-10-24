#include "ComponentMaterial.h"
#include "Application.h"
#include "ConsoleWindow.h"
#include "GeoPropertiesWindow.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ModuleImport.h"

void ComponentMaterial::DestroyTexture(const int& tex_ind)
{
	glDeleteTextures(1, &textures[tex_ind]->id_tex);

	if (!textures[tex_ind]->deleted)
	{
		for(int i = 0; i < textures[tex_ind]->referenced_mats.size(); i++)
		{ 
			for (int j = 0; j < textures[tex_ind]->referenced_mats[i]->textures.size(); j++)
			{ 
				if (textures[tex_ind]->referenced_mats[i]->textures[j]->filename == textures[tex_ind]->filename)
				{
					textures[tex_ind]->referenced_mats[i]->PopTexture(j);
			
				}
			}
				
		}
			

		delete textures[tex_ind];
	}

	textures[tex_ind] = nullptr;

	PopTexture(tex_ind);
}

void ComponentMaterial::PopTexture(const int& tex_index)
{
	for (int i = tex_index + 1; i < textures.size(); i++)
		textures[i - 1] = textures[i];

	textures.pop_back();


	if (textures.size() > 0)
		for (int i = 0; i < parent->meshes.size(); i++)
			if (this->parent->meshes[i]->Material_Ind >= textures.size())
				this->parent->meshes[i]->Material_Ind = -1;
}

void ComponentMaterial::CleanUp()
{
	for (int i = 0; i < textures.size(); i++)
		DestroyTexture(i);


	parent = nullptr;
}

Texture* ComponentMaterial::CheckTexRep(std::string name)
{
	Texture* ret = nullptr;

	GameObject* par = this->parent;

	while (par != NULL && ret == nullptr)
	{
		for (int i = 0; i < par->materials.size(); i++)
		{
			ret = par->materials[i]->CheckNameRep(name);
		}

		par = par->parent;
	}

	return ret;
}

Texture* ComponentMaterial::CheckNameRep(std::string name)
{
	Texture* ret = nullptr;

	for (int i = 0; i < this->textures.size(); i++)
	{
		if(name.substr(name.find_last_of("\\/") + 1) == textures[i]->filename)
			ret = textures[i];

		if (ret != nullptr)
			break;

		if(name.substr(name.find_last_of("\\/") + 1) == textures[i]->filename.substr(textures[i]->filename.find_last_of("\\/") + 1))
		ret = textures[i];
		if (ret != nullptr)
			break;

		if(name == textures[i]->filename)
			ret = textures[i];

		if (ret != nullptr)
			break;

		if(name == textures[i]->filename.substr(textures[i]->filename.find_last_of("\\/") + 1))
			ret = textures[i];
	}

	return ret;
}