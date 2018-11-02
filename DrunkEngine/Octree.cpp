#include "Octree.h"
#include "Application.h"

Octree::Octree(int elements_per_node)
{
	static_objs.clear();
	nodes.clear();

	this->elements_per_node = elements_per_node;

	RecursiveGetStaticObjs(App->mesh_loader->Root_Object);

	if (static_objs.size() > 0)
	{
		base_node = new Node(static_objs, nullptr, this);
		nodes.push_back(base_node);
	}
}

Octree::~Octree()
{
}

void Octree::Update()
{
	for (int i = 0; i < nodes.size(); i++)
		nodes[i]->Update();
}

void Octree::CleanUp()
{
	for (int i = 0; i < nodes.size(); i++)
		nodes[i]->CleanUp();

	static_objs.clear();
	nodes.clear();
}

void Octree::RecursiveGetStaticObjs(const GameObject * obj)
{
	for (int i = 0; i < obj->children.size(); i++)
	{
		if (obj->children[i]->is_static)
		{
			static_objs.push_back(obj->children[i]);
			RecursiveGetStaticObjs(obj->children[i]);
		}
	}
}

Octree::Node::Node(std::vector<GameObject*> objs_in_node, Node * parent, Octree * root)
{
	this->root = root;
	this->parent = parent;
	id = root->nodes.size();
	root->nodes.push_back(this);

	for (int i = 0; i < objs_in_node.size(); i++)
		this->objects_in_node.push_back(objs_in_node[i]);

	axis_to_check = Axis::Axis_X;

	SetNodeVertex();

	if (root->elements_per_node < objects_in_node.size())
		CreateNodes();
}

Octree::Node::Node(std::vector<GameObject*> objs_in_node, Node * parent, AABB bounding_box)
{
	this->root = parent->root;
	this->parent = parent;
	id = root->nodes.size();

	this->bounding_box = bounding_box;

	for (int i = 0; i < objs_in_node.size(); i++)
		this->objects_in_node.push_back(objs_in_node[i]);

	if (parent->axis_to_check == Axis::Axis_X)
		axis_to_check = Axis::Axis_Y;

	else if (parent->axis_to_check == Axis::Axis_Y)
		axis_to_check = Axis::Axis_Z;

	else if (parent->axis_to_check == Axis::Axis_Z)
		axis_to_check = Axis::Axis_X;

	if (root->elements_per_node < objects_in_node.size())
		CreateNodes();
}

Octree::Node::~Node()
{
}

void Octree::Node::Update()
{
	Draw();
}

void Octree::Node::Draw()
{
	//Draw node AABB
	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);

	glColor3f(0.f, 1.f, 1.f);

	glVertex3f(this->bounding_box.maxPoint.x, this->bounding_box.maxPoint.y, this->bounding_box.maxPoint.z);
	glVertex3f(this->bounding_box.maxPoint.x, this->bounding_box.minPoint.y, this->bounding_box.maxPoint.z);

	glVertex3f(this->bounding_box.maxPoint.x, this->bounding_box.maxPoint.y, this->bounding_box.maxPoint.z);
	glVertex3f(this->bounding_box.minPoint.x, this->bounding_box.maxPoint.y, this->bounding_box.maxPoint.z);

	glVertex3f(this->bounding_box.maxPoint.x, this->bounding_box.maxPoint.y, this->bounding_box.maxPoint.z);
	glVertex3f(this->bounding_box.maxPoint.x, this->bounding_box.maxPoint.y, this->bounding_box.minPoint.z);

	glVertex3f(this->bounding_box.maxPoint.x, this->bounding_box.minPoint.y, this->bounding_box.minPoint.z);
	glVertex3f(this->bounding_box.maxPoint.x, this->bounding_box.maxPoint.y, this->bounding_box.minPoint.z);

	glVertex3f(this->bounding_box.maxPoint.x, this->bounding_box.minPoint.y, this->bounding_box.minPoint.z);
	glVertex3f(this->bounding_box.maxPoint.x, this->bounding_box.minPoint.y, this->bounding_box.maxPoint.z);

	glVertex3f(this->bounding_box.maxPoint.x, this->bounding_box.minPoint.y, this->bounding_box.minPoint.z);
	glVertex3f(this->bounding_box.minPoint.x, this->bounding_box.minPoint.y, this->bounding_box.minPoint.z);

	glVertex3f(this->bounding_box.minPoint.x, this->bounding_box.maxPoint.y, this->bounding_box.minPoint.z);
	glVertex3f(this->bounding_box.minPoint.x, this->bounding_box.minPoint.y, this->bounding_box.minPoint.z);

	glVertex3f(this->bounding_box.minPoint.x, this->bounding_box.maxPoint.y, this->bounding_box.minPoint.z);
	glVertex3f(this->bounding_box.minPoint.x, this->bounding_box.maxPoint.y, this->bounding_box.maxPoint.z);

	glVertex3f(this->bounding_box.minPoint.x, this->bounding_box.maxPoint.y, this->bounding_box.minPoint.z);
	glVertex3f(this->bounding_box.maxPoint.x, this->bounding_box.maxPoint.y, this->bounding_box.minPoint.z);

	glVertex3f(this->bounding_box.minPoint.x, this->bounding_box.minPoint.y, this->bounding_box.maxPoint.z);
	glVertex3f(this->bounding_box.minPoint.x, this->bounding_box.maxPoint.y, this->bounding_box.maxPoint.z);

	glVertex3f(this->bounding_box.minPoint.x, this->bounding_box.minPoint.y, this->bounding_box.maxPoint.z);
	glVertex3f(this->bounding_box.maxPoint.x, this->bounding_box.minPoint.y, this->bounding_box.maxPoint.z);

	glVertex3f(this->bounding_box.minPoint.x, this->bounding_box.minPoint.y, this->bounding_box.maxPoint.z);
	glVertex3f(this->bounding_box.minPoint.x, this->bounding_box.minPoint.y, this->bounding_box.minPoint.z);

	glColor3f(0, 1, 0);

	glEnd();

	if (App->renderer3D->lighting)
		glEnable(GL_LIGHTING);
}

void Octree::Node::CleanUp()
{
	root = nullptr;
	parent = nullptr;
	child.clear();
	objects_in_node.clear();
}

void Octree::Node::SetNodeVertex()
{
	//Set vertex positions for this node
	vec object_center = objects_in_node[0]->getObjectCenter();

	bounding_box.maxPoint = object_center;
	bounding_box.minPoint = object_center;

	for (int i = 1; i < objects_in_node.size(); i++)
	{
		object_center = objects_in_node[i]->getObjectCenter();
		SetVertexPos(object_center);
	}
}

void Octree::Node::CreateNodes()
{
	switch (axis_to_check)
	{
	case Axis::Axis_X:
	{
		//	 X Y Z / -X Y Z
		vec center_1 = (bounding_box.maxPoint + vec(bounding_box.minPoint.x, bounding_box.maxPoint.y, bounding_box.maxPoint.z)) / 2;

		//	 X-Y-Z / -X-Y-Z
		vec center_2 = (vec(bounding_box.maxPoint.x, bounding_box.minPoint.y, bounding_box.minPoint.z) + bounding_box.minPoint) / 2;

		AABB new_AABB_Px = AABB(center_2, bounding_box.maxPoint);
		AABB new_AABB_Nx = AABB(bounding_box.minPoint, center_1);
		
		Node * Px = new Node(GetObjectsInNode(new_AABB_Px), this, new_AABB_Px);
		root->nodes.push_back(Px);
		child.push_back(Px);

		Node * Nx = new Node(GetObjectsInNode(new_AABB_Nx), this, new_AABB_Nx);
		root->nodes.push_back(Nx);
		child.push_back(Nx);

		break;
	}
	case Axis::Axis_Y:
	{
		//	 X Y Z /  X-Y Z
		vec center_1 = (bounding_box.maxPoint + vec(bounding_box.maxPoint.x, bounding_box.minPoint.y, bounding_box.maxPoint.z)) / 2;

		//	-X Y-Z / -X-Y-Z
		vec center_2 = (vec(bounding_box.minPoint.x, bounding_box.maxPoint.y, bounding_box.minPoint.z) + bounding_box.minPoint) / 2;

		AABB new_AABB_Py = AABB(center_2, bounding_box.maxPoint);
		AABB new_AABB_Ny = AABB(bounding_box.minPoint, center_1);

		Node * Py = new Node(GetObjectsInNode(new_AABB_Py), this, new_AABB_Py);
		root->nodes.push_back(Py);
		child.push_back(Py);

		Node * Ny = new Node(GetObjectsInNode(new_AABB_Ny), this, new_AABB_Ny);
		root->nodes.push_back(Ny);
		child.push_back(Ny);

		break;
	}
	case Axis::Axis_Z:
	{
		//	 X Y Z /  X Y-Z
		vec center_1 = (bounding_box.maxPoint + vec(bounding_box.maxPoint.x, bounding_box.maxPoint.y, bounding_box.minPoint.z)) / 2;

		//	-X-Y Z / -X-Y-Z 
		vec center_2 = (vec(bounding_box.minPoint.x, bounding_box.minPoint.y, bounding_box.maxPoint.z) + bounding_box.minPoint) / 2;

		AABB new_AABB_Pz = AABB(center_2, bounding_box.maxPoint);
		AABB new_AABB_Nz = AABB(bounding_box.minPoint, center_1);

		Node * Pz = new Node(GetObjectsInNode(new_AABB_Pz), this, new_AABB_Pz);
		root->nodes.push_back(Pz);
		child.push_back(Pz);

		Node * Nz = new Node(GetObjectsInNode(new_AABB_Nz), this, new_AABB_Nz);
		root->nodes.push_back(Nz);
		child.push_back(Nz);

		break;
	}
	}
}

std::vector<GameObject*> Octree::Node::GetObjectsInNode(AABB new_bounding_box)
{
	std::vector<GameObject*> objs_in_new_node;

	for (int i = 0; i < objects_in_node.size(); i++)
	{
		if (new_bounding_box.Contains(objects_in_node[i]->getObjectCenter()))
			objs_in_new_node.push_back(objects_in_node[i]);
	}

	return objs_in_new_node;
}

void Octree::Node::SetVertexPos(const vec object_center)
{
	if (bounding_box.maxPoint.x < object_center.x)
		bounding_box.maxPoint.x = object_center.x;
	
	if (bounding_box.minPoint.x > object_center.x)
		bounding_box.minPoint.x = object_center.x;

	if (bounding_box.maxPoint.y < object_center.y)
		bounding_box.maxPoint.y = object_center.y;
	
	if (bounding_box.minPoint.y > object_center.y)
		bounding_box.minPoint.y = object_center.y;
	
	if (bounding_box.maxPoint.z < object_center.z)
		bounding_box.maxPoint.z = object_center.z;
	
	if (bounding_box.minPoint.z > object_center.z)
		bounding_box.minPoint.z = object_center.z;
}
