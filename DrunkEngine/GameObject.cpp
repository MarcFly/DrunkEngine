#include "GameObject.h"
#include "Application.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentCamera.h"
#include "KdTree.h"

// Creation of Root Node from a file
GameObject::GameObject()
{
	SetUUID();
	//GetTransform();
}
GameObject::GameObject(GameObject * par, const char* name, CTypes type)
{
	this->name = name;
	this->root = this;

	if (par != nullptr)
	{	
		this->parent = par;
		root = par->root;
	}
	GetTransform();

	if (type == CT_Camera)
		this->components.push_back(new ComponentCamera(this));

	Start();
}
GameObject::GameObject(const char* path, const aiScene* scene, const aiNode * root_obj, const char * file_path, GameObject* par)
{
	SetUUID();

	this->name = file_path;
	this->root = this;	

	if (par != nullptr)
	{
		parent = par;
		root = par->root;
	}

	for (int i = 0; i < root_obj->mNumChildren; i++)
		this->children.push_back(App->importer->ImportGameObject(path, scene, root_obj->mChildren[i], this));

	GetTransform();

	Start();
}

void GameObject::Start()
{
	for (int i = 0; i < this->components.size(); i++)
		this->components[i]->Start();

	for (int i = 0; i < this->children.size(); i++)
		this->children[i]->Start();

	if (this->parent != nullptr)
	{
		SetBoundBox();
		SetTransformedBoundBox();
	}
}

void GameObject::Update(float dt)
{
	App->scene->getRootObj()->CalculateGlobalTransforms();

	for (int i = 0; i < this->components.size(); i++)
		this->components[i]->Update(dt);

	for (int i = 0; i < this->children.size(); i++)
		this->children[i]->Update(dt);

	if (Scene_KdTree != nullptr)
		Scene_KdTree->Update();

	Draw();

}

void GameObject::Draw()
{
	for (int i = 0; i < this->components.size(); i++)		
		this->components[i]->Draw();

	if (this->BoundingBox != nullptr && this->GetTransform()->update_bouding_box)
	{
		SetTransformedBoundBox();
		this->GetTransform()->update_bouding_box = false;
	}

	if (this->BoundingBox != nullptr && (App->renderer3D->bounding_box || this->active))
		this->DrawBB();


	for (int i = 0; i < this->children.size(); i++)
		this->children[i]->Draw();
}

void GameObject::DrawBB()
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
	if (this->BoundingBox == nullptr)
		this->SetBoundBox();

	float aux_x = (this->BoundingBox->maxPoint.x + this->BoundingBox->minPoint.x) / 2;
	float aux_y = (this->BoundingBox->maxPoint.y + this->BoundingBox->minPoint.y) / 2;
	float aux_z = (this->BoundingBox->maxPoint.z + this->BoundingBox->minPoint.z) / 2;

	return vec(aux_x, aux_y, aux_z);
}

float GameObject::SetBoundBox()
{
	max_distance_point = 0;

	this->BoundingBox = new AABB(vec(INT_MAX, INT_MAX, INT_MAX),vec(INT_MIN, INT_MIN, INT_MIN));

	if (this->children.size() == 0)
	{
		SetBoundBoxFromMeshes();
	}
	else
	{
		// Set Children Bounding Boxes
		for (int i = 0; i < this->children.size(); i++)
			this->children[i]->SetBoundBox();

		for (int i = 0; i < this->children.size(); i++)
		{

			// Setting the BB min and max points

			if (this->BoundingBox->maxPoint.x < this->children[i]->BoundingBox->maxPoint.x)
				this->BoundingBox->maxPoint.x = this->children[i]->BoundingBox->maxPoint.x;		

			if (this->BoundingBox->minPoint.x > this->children[i]->BoundingBox->minPoint.x)
				this->BoundingBox->minPoint.x = this->children[i]->BoundingBox->minPoint.x;	

			if (this->BoundingBox->maxPoint.y < this->children[i]->BoundingBox->maxPoint.y)
				this->BoundingBox->maxPoint.y = this->children[i]->BoundingBox->maxPoint.y;	

			if (this->BoundingBox->minPoint.y > this->children[i]->BoundingBox->minPoint.y)
				this->BoundingBox->minPoint.y = this->children[i]->BoundingBox->minPoint.y;

			if (this->BoundingBox->maxPoint.z < this->children[i]->BoundingBox->maxPoint.z)
				this->BoundingBox->maxPoint.z = this->children[i]->BoundingBox->maxPoint.z;	

			if (this->BoundingBox->minPoint.z > this->children[i]->BoundingBox->minPoint.z)
				this->BoundingBox->minPoint.z = this->children[i]->BoundingBox->minPoint.z;
		}
	}

	// Set Return Value
	{
		if (abs(this->BoundingBox->maxPoint.x) > max_distance_point) { max_distance_point = abs(this->BoundingBox->maxPoint.x); }
		if (abs(this->BoundingBox->maxPoint.y) > max_distance_point) { max_distance_point = abs(this->BoundingBox->maxPoint.y); }
		if (abs(this->BoundingBox->maxPoint.z) > max_distance_point) { max_distance_point = abs(this->BoundingBox->maxPoint.z); }
		if (abs(this->BoundingBox->minPoint.x) > max_distance_point) { max_distance_point = abs(this->BoundingBox->minPoint.x); }
		if (abs(this->BoundingBox->minPoint.y) > max_distance_point) { max_distance_point = abs(this->BoundingBox->minPoint.y); }
		if (abs(this->BoundingBox->minPoint.z) > max_distance_point) { max_distance_point = abs(this->BoundingBox->minPoint.z); }
	}

	return max_distance_point;
}

void GameObject::SetTransformedBoundBox()
{
	max_distance_point = 0;

	this->BoundingBox = new AABB(vec(INT_MAX, INT_MAX, INT_MAX), vec(INT_MIN, INT_MIN, INT_MIN));

	if (this->children.size() == 0)
	{
		std::vector<Component*> cmp_meshes;
		cmp_meshes = GetComponents(CT_Mesh);

		if (cmp_meshes.size() > 0)
		{


			for (int i = 0; i < cmp_meshes.size(); i++)
			{
				math::AABB auxBB = *cmp_meshes[i]->AsMesh()->BoundingBox;
				auxBB.TransformAsAABB(cmp_meshes[i]->AsMesh()->parent->GetTransform()->global_transform);

				// Setting the BB min and max points

				if (this->BoundingBox->maxPoint.x < auxBB.maxPoint.x)
					this->BoundingBox->maxPoint.x = auxBB.maxPoint.x;
				if (this->BoundingBox->minPoint.x > auxBB.minPoint.x)
					this->BoundingBox->minPoint.x = auxBB.minPoint.x;
				if (this->BoundingBox->maxPoint.y < auxBB.maxPoint.y)
					this->BoundingBox->maxPoint.y = auxBB.maxPoint.y;
				if (this->BoundingBox->minPoint.y > auxBB.minPoint.y)
					this->BoundingBox->minPoint.y = auxBB.minPoint.y;
				if (this->BoundingBox->maxPoint.z < auxBB.maxPoint.z)
					this->BoundingBox->maxPoint.z = auxBB.maxPoint.z;
				if (this->BoundingBox->minPoint.z > auxBB.minPoint.z)
					this->BoundingBox->minPoint.z = auxBB.minPoint.z;
			}
		}
		else
		{
			this->BoundingBox->maxPoint = this->GetTransform()->position + float3(1, 1, 1);
			this->BoundingBox->minPoint = this->GetTransform()->position + float3(-1, -1, -1);
		}
		
	}
	else
	{
		// Set Children Bounding Boxes
		for (int i = 0; i < this->children.size(); i++)
			if (this->children[i]->BoundingBox == nullptr)
				this->children[i]->SetBoundBox();

		for (int i = 0; i < this->children.size(); i++)
		{
			math::AABB auxBB = *this->children[i]->BoundingBox;

			// Setting the BB min and max points with transforms

			if (this->BoundingBox->maxPoint.x < auxBB.maxPoint.x)
				this->BoundingBox->maxPoint.x = auxBB.maxPoint.x;
			if (this->BoundingBox->minPoint.x > auxBB.minPoint.x)
				this->BoundingBox->minPoint.x = auxBB.minPoint.x;
			if (this->BoundingBox->maxPoint.y < auxBB.maxPoint.y)
				this->BoundingBox->maxPoint.y = auxBB.maxPoint.y;
			if (this->BoundingBox->minPoint.y > auxBB.minPoint.y)
				this->BoundingBox->minPoint.y = auxBB.minPoint.y;
			if (this->BoundingBox->maxPoint.z < auxBB.maxPoint.z)
				this->BoundingBox->maxPoint.z = auxBB.maxPoint.z;
			if (this->BoundingBox->minPoint.z > auxBB.minPoint.z)
				this->BoundingBox->minPoint.z = auxBB.minPoint.z;
		}
	}

	// Set Return Value
	{
		if (abs(this->BoundingBox->maxPoint.x) > max_distance_point) { max_distance_point = abs(this->BoundingBox->maxPoint.x); }
		if (abs(this->BoundingBox->maxPoint.y) > max_distance_point) { max_distance_point = abs(this->BoundingBox->maxPoint.y); }
		if (abs(this->BoundingBox->maxPoint.z) > max_distance_point) { max_distance_point = abs(this->BoundingBox->maxPoint.z); }
		if (abs(this->BoundingBox->minPoint.x) > max_distance_point) { max_distance_point = abs(this->BoundingBox->minPoint.x); }
		if (abs(this->BoundingBox->minPoint.y) > max_distance_point) { max_distance_point = abs(this->BoundingBox->minPoint.y); }
		if (abs(this->BoundingBox->minPoint.z) > max_distance_point) { max_distance_point = abs(this->BoundingBox->minPoint.z); }
	}
}

void GameObject::SetBoundBoxFromMeshes()
{
	this->BoundingBox = new AABB(vec(INT_MAX, INT_MAX, INT_MAX), vec(INT_MIN, INT_MIN, INT_MIN));

	std::vector<Component*> cmp_meshes;
	cmp_meshes = GetComponents(CT_Mesh);

	if (cmp_meshes.size() > 0)
	{
		for (int i = 0; i < cmp_meshes.size(); i++)
		{
			// Setting the BB min and max points

			if (this->BoundingBox->maxPoint.x < cmp_meshes[i]->AsMesh()->BoundingBox->maxPoint.x)
				this->BoundingBox->maxPoint.x = cmp_meshes[i]->AsMesh()->BoundingBox->maxPoint.x;
			if (this->BoundingBox->minPoint.x > cmp_meshes[i]->AsMesh()->BoundingBox->minPoint.x)
				this->BoundingBox->minPoint.x = cmp_meshes[i]->AsMesh()->BoundingBox->minPoint.x;
			if (this->BoundingBox->maxPoint.y < cmp_meshes[i]->AsMesh()->BoundingBox->maxPoint.y)
				this->BoundingBox->maxPoint.y = cmp_meshes[i]->AsMesh()->BoundingBox->maxPoint.y;
			if (this->BoundingBox->minPoint.y > cmp_meshes[i]->AsMesh()->BoundingBox->minPoint.y)
				this->BoundingBox->minPoint.y = cmp_meshes[i]->AsMesh()->BoundingBox->minPoint.y;
			if (this->BoundingBox->maxPoint.z < cmp_meshes[i]->AsMesh()->BoundingBox->maxPoint.z)
				this->BoundingBox->maxPoint.z = cmp_meshes[i]->AsMesh()->BoundingBox->maxPoint.z;
			if (this->BoundingBox->minPoint.z > cmp_meshes[i]->AsMesh()->BoundingBox->minPoint.z)
				this->BoundingBox->minPoint.z = cmp_meshes[i]->AsMesh()->BoundingBox->minPoint.z;
		}
	}
	else
	{
		this->BoundingBox->maxPoint = this->GetTransform()->position + float3(1, 1, 1);
		this->BoundingBox->minPoint = this->GetTransform()->position + float3(-1, -1, -1);
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

void GameObject::CalculateGlobalTransforms()
{
	if (this->parent != nullptr)
		this->GetTransform()->global_transform = this->parent->GetTransform()->global_transform * this->GetTransform()->local_transform;

	else
		GetTransform()->global_transform = GetTransform()->local_transform;
	
	if (this->children.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = this->children.begin(); it != this->children.end(); it++)
		{
			(*it)->CalculateGlobalTransforms();
		}
	}
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

void GameObject::Load(JSON_Value* go, const char* file)
{
	JSON_Object* curr = json_value_get_object(json_object_get_value_at(json_value_get_object(go),0));

	this->UUID = json_object_dotget_number(curr, "UUID");
	this->par_UUID = json_object_get_number(curr, "par_UUID");
	this->name = json_object_get_string(curr, "name");

	JSON_Array* get_array = json_object_get_array(curr, "components");

	for (int i = 0; i < json_array_get_count(get_array); i++)
	{
		JSON_Value* val = json_object_get_value(json_value_get_object(json_array_get_value(get_array, i)),"properties");
		JSON_Object* obj = json_value_get_object(val);
		CTypes type = (CTypes)(int)json_object_get_number(obj, "type");

		if (type == 0)
			bool test = true;

		Component* add = NewComponent(type);
		add->Load(obj);
		add->parent = this;
		this->components.push_back(add);
	}
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
		ret[0] += aux->num_vertex;
		ret[1] += aux->num_faces;
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

Component* GameObject::NewComponent(CTypes type)
{
	if (type == CT_Mesh)
		return new ComponentMesh(this);
	else if (type == CT_Material)
		return new ComponentMaterial(this);
	else if (type == CT_Camera)
		return new ComponentCamera(this);
	else if (type == CT_Transform)
		return new ComponentTransform(this);
	
	return nullptr;
}

bool GameObject::isInsideFrustum(const ComponentCamera * cam, const AABB * bounding_box)
{
	float3 vCorner[8];
	int iTotalIn = 0;
	Plane planes[6];
	cam->frustum.GetPlanes(planes);

	bounding_box->GetCornerPoints(vCorner); // get the corners of the box into the vCorner array
	// test all 8 corners against the 6 sides
	// if all points are behind 1 specific plane, we are out
	// if we are in with all points, then we are fully in
	for (int p = 0; p < 6; ++p) {
		int iInCount = 8;
		int iPtIn = 1;
		for (int i = 0; i < 8; ++i) {
			// test this point against the planes
			if (!planes[p].AreOnSameSide(vCorner[i], cam->frustum.CenterPoint())) {
				iPtIn = 0;
				--iInCount;
			}
		}
		// were all the points outside of plane p?
		if (iInCount == 0)
			return false;
		// check if they were all on the right side of the plane
		iTotalIn += iPtIn;
	}
	// so if iTotalIn is 6, then all are inside the view
	if (iTotalIn == 6)
		return true;
	// we must be partly in then otherwise
	return true;
}