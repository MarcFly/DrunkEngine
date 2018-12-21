#include "GameObject.h"
#include "Application.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentCamera.h"
#include "ResourceMesh.h"
#include "ComponentBillboard.h"
#include "ComponentSkeleton.h"
#include "ComponentAnimation.h"

// Creation of Root Node from a file
GameObject::GameObject()
{
	GetTransform();

	App->gameObj->objects_in_scene.push_back(this);
	App->gameObj->non_static_objects_in_scene.push_back(this);

	Start();

	UUID = GetUUID();
}
GameObject::GameObject(const char* name, GameObject * par)
{
	this->name = name;
	this->root = this;

	if (par != nullptr)
	{	
		this->parent = par;
		root = par->root;
	}

	GetTransform();

	App->gameObj->objects_in_scene.push_back(this);
	App->gameObj->non_static_objects_in_scene.push_back(this);

	UUID = GetUUID();

	Start();
}

void GameObject::Start()
{
	for (int i = 0; i < this->components.size(); i++)
		this->components[i]->Start();

	for (int i = 0; i < this->children.size(); i++)
		this->children[i]->Start();
}

void GameObject::Update(float dt)
{
	for (int i = 0; i < this->components.size(); i++)
	{
		if (this->components[i]->active)
		{
			this->components[i]->Update(dt);
		}
	}

	for (int i = 0; i < this->children.size(); i++)
	{
		if(this->children[i]->active)
		{
			this->children[i]->Update(dt);
		}
	}

	Draw();
}

void GameObject::Draw()
{
	for (int i = 0; i < this->components.size(); i++)
	{
		if (this->components[i]->active)
		{
			this->components[i]->Draw();
		}
	}

	if (this->BoundingBox != nullptr && (App->renderer3D->bounding_box || this->sv_active))
		this->DrawBB();
}

void GameObject::DrawBB() const
{
	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);

	glColor3f(0.f, 1.f, 0.f);

	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->maxPoint.z);
	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->maxPoint.z);

	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->maxPoint.z);
	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->maxPoint.z);

	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->maxPoint.z);
	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->minPoint.z);

	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->minPoint.z);
	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->minPoint.z);

	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->minPoint.z);
	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->maxPoint.z);

	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->minPoint.z);
	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->minPoint.z);

	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->minPoint.z);
	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->minPoint.z);

	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->minPoint.z);
	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->maxPoint.z);

	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->minPoint.z);
	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->minPoint.z);

	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->maxPoint.z);
	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->maxPoint.y, this->BoundingBox->maxPoint.z);

	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->maxPoint.z);
	glVertex3f(this->BoundingBox->maxPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->maxPoint.z);

	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->maxPoint.z);
	glVertex3f(this->BoundingBox->minPoint.x, this->BoundingBox->minPoint.y, this->BoundingBox->minPoint.z);

	glColor3f(0, 1, 0);

	glEnd();

	if (App->renderer3D->lighting)
		glEnable(GL_LIGHTING);
}

vec GameObject::getObjectCenter()
{
	if (BoundingBox != nullptr)
	{
		float aux_x = (BoundingBox->maxPoint.x + BoundingBox->minPoint.x) / 2;
		float aux_y = (BoundingBox->maxPoint.y + BoundingBox->minPoint.y) / 2;
		float aux_z = (BoundingBox->maxPoint.z + BoundingBox->minPoint.z) / 2;

		return vec(aux_x, aux_y, aux_z);
	}

	return vec(0, 0, 0);
}

void GameObject::OrderChildren()
{
	for (int i = 0; i < this->children.size(); i++)
	{
		if (this->children[i]->par_UUID != UINT_FAST32_MAX && this->children[i]->par_UUID != this->children[i]->parent->UUID)
		{
			GameObject* obj = this->children[i];
			this->children[i]->to_pop = true;
			this->AdjustObjects();
			obj->to_pop = false;
			GameObject* get = this->GetChild(obj->par_UUID);
			obj->parent = get;
			if (get != nullptr)
				get->children.push_back(obj);
			i--;
		}
	}
}

void GameObject::AdjustObjects()
{
	int i = 0;
	for (; i < this->children.size(); i++)
	{
		if (this->children[i]->to_pop == true)
		{
			//delete this->children[i];
			//this->children[i] = nullptr;
			break;
		}
	}

	for (int j = i; j < this->children.size() - 1; j++)
	{
		this->children[j] = this->children[j + 1];
	}

	this->children.pop_back();
}

void GameObject::AdjustComponents()
{
	int i = 0;
	for (; i < this->components.size(); i++)
	{
		if (this->components[i]->to_pop == true)
		{
			delete this->components[i];
			this->components[i] = nullptr;
			break;
		}
	}

	for (int j = i; j < this->components.size() - 1; j++)
	{
		this->components[j] = this->components[j + 1];
	}

	this->components.pop_back();
}

void GameObject::RecursiveSetStatic(GameObject * obj, const bool bool_static)
{
	obj->is_static = bool_static;

	for (int i = 0; i < obj->children.size(); i++)
		RecursiveSetStatic(obj->children[i], bool_static);
}

void GameObject::CleanUp()
{
	for (int i = 0; i < this->components.size(); i++)
	{
		this->components[i]->CleanUp();
		delete this->components[i];
		this->components[i] = nullptr;
	}
	this->components.clear();

	if (this->BoundingBox != nullptr) {
		delete this->BoundingBox;
		this->BoundingBox = nullptr;
	}

	for (int i = 0; i < this->children.size(); i++)
		this->children[i]->CleanUp();

	this->children.clear();

	this->parent = nullptr;
	this->root = nullptr;
	this->name.clear();
}

void GameObject::DestroyThisObject()
{
	this->CleanUp();
	this->to_pop = true;

	if (parent != nullptr)
		parent->AdjustObjects();
}

void GameObject::DestroyComponent()
{

}

void GameObject::RecursiveSetNewUUID()
{
	if (this->parent == nullptr)
		par_UUID = UINT_FAST32_MAX;
	else
		par_UUID = parent->UUID;
	
	UUID = GetUUID();

	for (int i = 0; i < children.size(); i++)
		children[i]->RecursiveSetNewUUID();

}

void GameObject::Load(const JSON_Value* go, const char* file)
{
	

}

void GameObject::Save(JSON_Array* go)
{
	JSON_Value* append = json_value_init_object();
	JSON_Object* curr = json_value_get_object(append);

	std::string obj = "gameobject.";
	std::string set_val;
	
	set_val = obj + "UUID";
	json_object_dotset_number(curr, set_val.c_str(), UUID);

	set_val = obj + "par_UUID";
	json_object_dotset_number(curr, set_val.c_str(), par_UUID);

	set_val = obj + "name";
	json_object_dotset_string(curr, set_val.c_str(), name.c_str());

	JSON_Value* set_array = json_value_init_array();
	JSON_Array* comps = json_value_get_array(set_array);

	for (int i = 0; i < this->components.size(); i++)
		this->components[i]->Save(comps);

	set_val = obj + "components";
	json_object_dotset_value(curr,set_val.c_str(), set_array);

	json_array_append_value(go, append);

	for(int i = 0; i < this->children.size(); i++)
		this->children[i]->Save(go);
}

std::vector<uint> GameObject::GetMeshProps()
{
	uint ret[2] = { 0,0 };
	std::vector<Component*> meshes;
	meshes = GetComponents(CT_Mesh);
	for (int i = 0; i < meshes.size(); i++)
	{
		ComponentMesh* aux = meshes[i]->AsMesh();
		ret[0] += aux->r_mesh->num_vertex;
		ret[1] += aux->r_mesh->num_faces;
	}

	for (int i = 0; i < children.size(); i++)
	{
		std::vector<uint> child = children[i]->GetMeshProps();
		ret[0] += child[0];
		ret[1] += child[1];
	}

	std::vector<uint> ret_v;
	ret_v.push_back(ret[0]);
	ret_v.push_back(ret[1]);

	return ret_v;
}

Component* GameObject::NewComponent(const CTypes type)
{
	if (type == CT_Mesh)
		return new ComponentMesh(this);
	else if (type == CT_Material)
		return new ComponentMaterial(this);
	else if (type == CT_Camera)
		return new ComponentCamera(this);
	else if (type == CT_Transform)
		return new ComponentTransform(this);
	/*else if (type == CT_Billboard)
		return new ComponentBillboard(this);*/
	else if (type == CT_Skeleton)
		return new ComponentSkeleton(this);
	else if (type == CT_Animation)
		return new ComponentAnimation(this);
	
	return nullptr;
}