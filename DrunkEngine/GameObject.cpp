#include "GameObject.h"
#include "Application.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentCamera.h"


// Creation of Root Node from a file
GameObject::GameObject()
{
	SetUUID();
	components.push_back(new ComponentTransform(this));
}
GameObject::GameObject(const char* path, const aiScene* scene, const aiNode * root_obj, const char * file_path)
{
	SetUUID();

	this->name = "Scene";
	this->root = this;

	this->children.push_back(App->importer->ImportGameObject(path, scene, root_obj, this));

	if (this->parent == nullptr)
		this->camera = new ComponentCamera(this);

	Start();
}

void GameObject::Start()
{
	if (camera != nullptr)
		camera->Start();

	for (int i = 0; i < this->components.size(); i++)
		this->components[i]->Start();

}

void GameObject::Update(float dt)
{
	if (camera != nullptr)
		camera->Update(dt);

	for (int i = 0; i < this->components.size(); i++)
		this->components[i]->Update();

	Draw();

}

void GameObject::Draw()
{
	for (int i = 0; i < this->components.size(); i++)
		this->components[i]->Draw();

	if ((App->renderer3D->bounding_box || this->active) && this->BoundingBox != nullptr)
		this->DrawBB();

	for (int i = 0; i < this->children.size(); i++)
		this->children[i]->Draw();
		
	if (camera != nullptr)
		camera->Draw();
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
	float ret = 0;

	this->BoundingBox = new AABB(vec(INT_MAX, INT_MAX, INT_MAX),vec(INT_MIN, INT_MIN, INT_MIN));

	if (this->children.size() == 0)
	{
		SetBoundBoxFromMeshes();
	}
	else
	{
		// Set Children Bounding Boxes if they don't have one
		for (int i = 0; i < this->children.size(); i++)
			if(this->children[i]->BoundingBox == nullptr)
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
	
	if (abs(this->BoundingBox->maxPoint.x) > ret) { ret = abs(this->BoundingBox->maxPoint.x); }
	if (abs(this->BoundingBox->maxPoint.y) > ret) { ret = abs(this->BoundingBox->maxPoint.y); }
	if (abs(this->BoundingBox->maxPoint.z) > ret) { ret = abs(this->BoundingBox->maxPoint.z); }
	if (abs(this->BoundingBox->minPoint.x) > ret) { ret = abs(this->BoundingBox->minPoint.x); }
	if (abs(this->BoundingBox->minPoint.y) > ret) { ret = abs(this->BoundingBox->minPoint.y); }
	if (abs(this->BoundingBox->minPoint.z) > ret) { ret = abs(this->BoundingBox->minPoint.z); }
	

	return ret;
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
}

void GameObject::AdjustObjects()
{
	int i = 0;
	for (; i < this->children.size(); i++)
	{
		if (this->children[i]->to_pop == true)
		{
			delete this->children[i];
			this->children[i] = nullptr;
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


void GameObject::Load(JSON_Value* curr, const char* file)
{

}

void GameObject::Save(JSON_Value* scene, const char* file)
{
	JSON_Object* curr = json_value_get_object(scene);

	std::string obj = std::to_string(this->UUID) + ".";
	std::string set_val;
	
	set_val = obj + "par_UUID";
	json_object_dotset_number(curr, set_val.c_str(), par_UUID);

	set_val = obj + "name";
	json_object_dotset_string(curr, set_val.c_str(), name.c_str());

	json_serialize_to_file(scene, file);
	scene = json_parse_file(file);

	for (int i = 0; i < this->components.size(); i++)
		this->components[i]->Save(scene, file);

	for(int i = 0; i < this->children.size(); i++)
		this->children[i]->Save(scene, file);
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