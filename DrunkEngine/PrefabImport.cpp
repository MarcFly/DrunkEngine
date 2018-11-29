#include "PrefabImport.h"
#include "ResourcePrefab.h"
#include "parson/parson.h"
#include "Application.h"

void CopyPrefab(DGUID fID, GameObject* gobj)
{
	//We are not yet creating prefabs
}

ResourcePrefab* PrefabImport::LoadPrefab(const char* file)
{
	ResourcePrefab* r_prefab = new ResourcePrefab();

	//We are not yet creating prefabs

	return r_prefab;
}

void PrefabImport::ExportAINode(const aiScene* scene, const aiNode* node, JSON_Array* go, const Uint32 par_UUID, const char* name)
{
	JSON_Value* append = json_value_init_object();
	JSON_Object* curr = json_value_get_object(append);

	if (!IgnoredNodes(node->mName.C_Str()))
	{
		std::string obj = "gameobject.";
		std::string set_val;

		set_val = obj + "UUID";
		Uint32 UUID = GetUUID();
		json_object_dotset_number(curr, set_val.c_str(), UUID);

		set_val = obj + "par_UUID";
		json_object_dotset_number(curr, set_val.c_str(), par_UUID);

		set_val = obj + "name";
		if (node->mParent == NULL)
			json_object_dotset_string(curr, set_val.c_str(), name);
		else
			json_object_dotset_string(curr, set_val.c_str(), node->mName.C_Str());

		JSON_Value* set_array = json_value_init_array();
		JSON_Array* comps = json_value_get_array(set_array);

		// Minimal Component Exports to be loaded from .prefabdrnk file
		{
			ExportTransformNode(comps, &node->mTransformation);

			for (int i = 0; i < node->mNumMeshes; i++)
			{
				ExportMeshNode(comps, scene->mMeshes[node->mMeshes[i]], node->mMeshes[i], name);

				if (scene->mMeshes[node->mMeshes[i]]->mMaterialIndex != -1)
					ExportMatNode(comps, scene->mMaterials[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex], scene->mMeshes[node->mMeshes[i]]->mMaterialIndex, name);

				if (scene->mMeshes[node->mMeshes[i]]->HasBones())
					ExportBonesNode(comps, scene->mMeshes[node->mMeshes[i]], node->mMeshes[i], name);
			}
		}

		set_val = obj + "components";
		json_object_dotset_value(curr, set_val.c_str(), set_array);

		// Adding GO to file
		json_array_append_value(go, append);

		for (int i = 0; i < node->mNumChildren; i++)
			ExportAINode(scene, node->mChildren[i], go, UUID, name);
	}
	else
	{
		for (int i = 0; i < node->mNumChildren; i++)
			ExportAINode(scene, node->mChildren[i], go, par_UUID, name);
	}
}

void PrefabImport::ExportTransformNode(JSON_Array* comps, const aiMatrix4x4* trans)
{
	JSON_Value* append = json_value_init_object();
	JSON_Object* curr = json_value_get_object(append);

	aiVector3D local_scale;
	aiVector3D pos;
	aiQuaternion rot;

	trans->Decompose(local_scale, rot, pos);

	float3 scale = float3(local_scale.x, local_scale.y, local_scale.z);
	Quat rotate_quat = Quat(rot.x, rot.y, rot.z, rot.w);
	float3 position = float3(pos.x, pos.y, pos.z);

	json_object_dotset_number(curr, "properties.type", CT_Transform);

	json_object_dotset_number(curr, "properties.position.x", position.x);
	json_object_dotset_number(curr, "properties.position.y", position.y);
	json_object_dotset_number(curr, "properties.position.z", position.z);

	json_object_dotset_number(curr, "properties.scale.x", scale.x);
	json_object_dotset_number(curr, "properties.scale.y", scale.y);
	json_object_dotset_number(curr, "properties.scale.z", scale.z);

	json_object_dotset_number(curr, "properties.rotate_quat.w", rotate_quat.w);
	json_object_dotset_number(curr, "properties.rotate_quat.x", rotate_quat.x);
	json_object_dotset_number(curr, "properties.rotate_quat.y", rotate_quat.y);
	json_object_dotset_number(curr, "properties.rotate_quat.z", rotate_quat.z);

	json_object_dotset_boolean(curr, "properties.fromAINode", true);

	json_array_append_value(comps, append);
}

void PrefabImport::ExportMeshNode(JSON_Array* comps, const aiMesh* mesh, const int& mesh_id, std::string name)
{
	JSON_Value* append = json_value_init_object();
	JSON_Object* curr = json_value_get_object(append);

	json_object_dotset_number(curr, "properties.type", CT_Mesh);
	
	std::string filename = ".\\Library\\" + name + "_Mesh_" + std::to_string(mesh_id) + ".meshdrnk";
	json_object_dotset_string(curr, "properties.filename", filename.c_str());

	json_array_append_value(comps, append);
}

void PrefabImport::ExportMatNode(JSON_Array* comps, const aiMaterial* mat, const int& mat_id, std::string name)
{
	JSON_Value* append = json_value_init_object();
	JSON_Object* curr = json_value_get_object(append);

	json_object_dotset_number(curr, "properties.type", CT_Material);
	
	std::string filename = ".\\Library\\" + name + "_Mat_" + std::to_string(mat_id) + ".matdrnk";
	json_object_dotset_string(curr, "properties.filename", filename.c_str());

	json_array_append_value(comps, append);
}

void PrefabImport::ExportBonesNode(JSON_Array* comps, const aiMesh* mesh, const int& mesh_id, std::string name)
{
	JSON_Value* append = json_value_init_object();
	JSON_Object* curr = json_value_get_object(append);

	json_object_dotset_number(curr, "properties.type", CT_Skeleton);

	std::string filename = ".\\Library\\" + name + "_Mesh_" + std::to_string(mesh_id) + "_Skel.skeldrnk";
	json_object_dotset_string(curr, "properties.filename", filename.c_str());

	json_array_append_value(comps, append);
}

void PrefabImport::ExportMeta(const aiNode* obj, std::string& path)
{

}

void PrefabImport::LoadMeta(const char* file, MetaPrefab* meta)
{

}

//------------------------------------

GameObject * PrefabImport::ImportGameObject(const char* path, JSON_Value* go)
{
	GameObject* ret = new GameObject();

	// Basically to get to the current gameobject
	// You have to enter 2 values (CurrVal->Obj->Arry of objs Val->Curr Obj)
	JSON_Object* curr = json_value_get_object(json_object_get_value_at(json_value_get_object(go), 0));
	if (curr != nullptr)
	{
		ret->UUID = json_object_dotget_number(curr, "UUID");
		ret->par_UUID = json_object_get_number(curr, "par_UUID");
		ret->name = json_object_get_string(curr, "name");

		JSON_Array* comps = json_object_get_array(curr, "components");

		for (int i = 0; i < json_array_get_count(comps); i++)
		{
			JSON_Value* val = json_object_get_value(json_value_get_object(json_array_get_value(comps, i)), "properties");
			JSON_Object* obj = json_value_get_object(val);

			CTypes type = (CTypes)(int)json_object_get_number(obj, "type");
			Component* add = nullptr;
			if (type == CT_Transform)
				add = ret->GetTransform();
			else
				add = ret->NewComponent(type);
			if (type == 5)
				bool ret = false;
			add->Load(obj);
			add->parent = ret;
			if (type != CT_Transform)
				ret->components.push_back(add);
		}

		// Free GO Object
		json_array_clear(comps);
		json_object_clear(curr);
	}
	
	return ret;
}

bool PrefabImport::IgnoredNodes(const char* node_name)
{
	std::string test = std::string(node_name).substr(0,3);
	int cmp = StringToInt(test);

	bool ret = false;

	if (cmp <= IN_Error || cmp >= IN_Max)
		ret = IgnoredStrings(node_name);
	else if (cmp >= IN_NOT_IGNORED)
		ret = false;
	else if (cmp == IN_Joint)
			ret = true;
	else if (cmp == IN_Handle)
		ret = true;
	
	return ret;
}

bool PrefabImport::IgnoredStrings(const char* node_name)
{
	bool ret = false;

	std::string cmp_str = node_name;
	if (cmp_str.find("joint") != std::string::npos)
		ret = true;
	else if (cmp_str.find("handle") != std::string::npos)
		ret = true;

	return ret;
}