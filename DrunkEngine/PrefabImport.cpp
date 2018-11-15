#include "PrefabImport.h"
#include "ResourcePrefab.h"
#include "parson/parson.h"

void CopyPrefab(DGUID fID, GameObject* gobj)
{

}

ResourcePrefab* PrefabImport::LoadPrefab(const char* file)
{
	ResourcePrefab* r_prefab = new ResourcePrefab();


	return r_prefab;
}

void PrefabImport::ExportAINode(const aiNode* node, JSON_Array* go, const Uint32 par_UUID)
{
	JSON_Value* append = json_value_init_object();
	JSON_Object* curr = json_value_get_object(append);

	std::string obj = "gameobject.";
	std::string set_val;

	set_val = obj + "UUID";
	Uint32 UUID = GetUUID();
	json_object_dotset_number(curr, set_val.c_str(), UUID);

	set_val = obj + "par_UUID";
	json_object_dotset_number(curr, set_val.c_str(), par_UUID);

	set_val = obj + "name";
	json_object_dotset_string(curr, set_val.c_str(), node->mName.C_Str());

	JSON_Value* set_array = json_value_init_array();
	JSON_Array* comps = json_value_get_array(set_array);

	{
		ComponentTransform to_save(&node->mTransformation, nullptr);
		to_save.Save(comps);

		for (int i = 0; i < node->mNumMeshes; i++)
		{
			node->mMeshes[i] ;
		}
	}
	
	set_val = obj + "components";
	json_object_dotset_value(curr, set_val.c_str(), set_array);

	json_array_append_value(go, append);

	for (int i = 0; i < node->mNumChildren; i++)
		ExportAINode(node->mChildren[i], go, UUID);


}

void ExportMeta(const aiNode* obj, std::string& path)
{

}

void LoadMeta(const char* file, MetaPrefab* meta)
{

}
