#include "KDTree.h"
#include "Application.h"

KDTree::KDTree(int elements_per_node, int max_subdivisions)
{
	static_objs.clear();
	nodes.clear();

	this->elements_per_node = elements_per_node;
	this->max_subdivisions = max_subdivisions;

	RecursiveGetStaticObjs(App->gameObj->getRootObj());

	if (static_objs.size() > 0)
	{
		base_node = new Node(static_objs, nullptr, this);
		nodes.push_back(base_node);
	}
}

KDTree::~KDTree()
{
}

void KDTree::Update()
{
	for (int i = 0; i < nodes.size(); i++)
		nodes[i]->Update();
}

void KDTree::CleanUp()
{
	for (int i = 0; i < nodes.size(); i++)
		nodes[i]->CleanUp();

	static_objs.clear();
	nodes.clear();
}

void KDTree::RecursiveGetStaticObjs(const GameObject * obj)
{
	for (int i = 0; i < obj->children.size(); i++)
	{
		if (obj->children[i]->children.size() > 0)
			RecursiveGetStaticObjs(obj->children[i]);

		if (obj->children[i]->is_static && (obj->children[i]->GetComponent(CTypes::CT_Camera) != nullptr || obj->children[i]->GetComponent(CTypes::CT_Mesh) != nullptr))
		{
			static_objs.push_back(obj->children[i]);
		}
	}
}

KDTree::Node::Node(std::vector<GameObject*>& objs_in_node, Node * parent, KDTree * root)
{
	this->root = root;
	this->parent = parent;
	id = root->nodes.size();
	subdivision = 1;
	root->nodes.push_back(this);

	for (int i = 0; i < objs_in_node.size(); i++)
		this->objects_in_node.push_back(objs_in_node[i]);

	axis_to_check = Axis::Axis_X;

	SetNodeVertex();

	if (root->elements_per_node < objects_in_node.size() && subdivision < root->max_subdivisions)
		CreateNodes();
}

KDTree::Node::Node(std::vector<GameObject*>& objs_in_node, Node * parent, AABB bounding_box)
{
	this->root = parent->root;
	this->parent = parent;
	id = root->nodes.size();
	subdivision = 1 + parent->subdivision;

	this->bounding_box = bounding_box;

	for (int i = 0; i < objs_in_node.size(); i++)
		this->objects_in_node.push_back(objs_in_node[i]);

	if (parent->axis_to_check == Axis::Axis_X)
		axis_to_check = Axis::Axis_Y;

	else if (parent->axis_to_check == Axis::Axis_Y)
		axis_to_check = Axis::Axis_Z;

	else if (parent->axis_to_check == Axis::Axis_Z)
		axis_to_check = Axis::Axis_X;

	if (root->elements_per_node < objects_in_node.size() && subdivision < root->max_subdivisions)
		CreateNodes();
}

KDTree::Node::~Node()
{
}

void KDTree::Node::Update()
{
	Draw();
}

void KDTree::Node::Draw()
{
	//Draw node AABB
	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);

	if (axis_to_check == Axis_X)
		glColor3f(0.f, 1.f, 1.f);

	else if (axis_to_check == Axis_Y)
		glColor3f(1.f, 1.f, 0.f);

	else if (axis_to_check == Axis_Z)
		glColor3f(1.f, 0.f, 1.f);

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

void KDTree::Node::CleanUp()
{
	root = nullptr;
	parent = nullptr;
	child.clear();
	objects_in_node.clear();
}

void KDTree::Node::SetNodeVertex()
{
	//Set vertex positions for this node
	bounding_box.maxPoint = vec(INT_MIN,INT_MIN,INT_MIN);
	bounding_box.minPoint = vec(INT_MAX,INT_MAX,INT_MAX);

	for (int i = 0; i < objects_in_node.size(); i++)
	{
		/*vec object_center = objects_in_node[i]->getObjectCenter();
		SetVertexPos(object_center);*/

		SetVertexPos(objects_in_node[i]->BoundingBox->minPoint, objects_in_node[i]->BoundingBox->maxPoint);

	}
}

void KDTree::Node::CreateNodes()
{
	vec center_1;
	vec center_2;
	AABB new_AABB_Node1;
	AABB new_AABB_Node2;

	if (axis_to_check == Axis_X)
	{
		//	 X Y Z / -X Y Z
		vec center_1 = (bounding_box.maxPoint + vec(bounding_box.minPoint.x, bounding_box.maxPoint.y, bounding_box.maxPoint.z)) / 2;
		//	 X-Y-Z / -X-Y-Z
		vec center_2 = (vec(bounding_box.maxPoint.x, bounding_box.minPoint.y, bounding_box.minPoint.z) + bounding_box.minPoint) / 2;

		new_AABB_Node1 = AABB(vec(GetKdTreeCut(axis_to_check), center_2.y, center_2.z), bounding_box.maxPoint);
		new_AABB_Node2 = AABB(bounding_box.minPoint, vec(GetKdTreeCut(axis_to_check), center_1.y, center_1.z));
	}

	else if (axis_to_check == Axis_Y)
	{
		//	 X Y Z /  X-Y Z
		vec center_1 = (bounding_box.maxPoint + vec(bounding_box.maxPoint.x, bounding_box.minPoint.y, bounding_box.maxPoint.z)) / 2;
		//	-X Y-Z / -X-Y-Z
		vec center_2 = (vec(bounding_box.minPoint.x, bounding_box.maxPoint.y, bounding_box.minPoint.z) + bounding_box.minPoint) / 2;

		new_AABB_Node1 = AABB(vec(center_2.x, GetKdTreeCut(axis_to_check), center_2.z), bounding_box.maxPoint);
		new_AABB_Node2 = AABB(bounding_box.minPoint, vec(center_2.x, GetKdTreeCut(axis_to_check), center_1.z));
	}

	else  // Axis_Z
	{
		//	 X Y Z /  X Y-Z
		vec center_1 = (bounding_box.maxPoint + vec(bounding_box.maxPoint.x, bounding_box.maxPoint.y, bounding_box.minPoint.z)) / 2;
		//	-X-Y Z / -X-Y-Z 
		vec center_2 = (vec(bounding_box.minPoint.x, bounding_box.minPoint.y, bounding_box.maxPoint.z) + bounding_box.minPoint) / 2;

		new_AABB_Node1 = AABB(vec(center_2.x, center_2.y, GetKdTreeCut(axis_to_check)), bounding_box.maxPoint);
		new_AABB_Node2 = AABB(bounding_box.minPoint, vec(center_2.x, center_1.y, GetKdTreeCut(axis_to_check)));
	}

	Node * Node1 = new Node(GetObjectsInNode(new_AABB_Node1), this, new_AABB_Node1);
	root->nodes.push_back(Node1);
	child.push_back(Node1);

	Node * Node2 = new Node(GetObjectsInNode(new_AABB_Node2), this, new_AABB_Node2);
	root->nodes.push_back(Node2);
	child.push_back(Node2);

}

std::vector<GameObject*> KDTree::Node::GetObjectsInNode(AABB& new_bounding_box)
{
	std::vector<GameObject*> objs_in_new_node;

	for (int i = 0; i < objects_in_node.size(); i++)
	{
		if (new_bounding_box.Intersects(*objects_in_node[i]->BoundingBox))
			objs_in_new_node.push_back(objects_in_node[i]);
	}

	return objs_in_new_node;
}

void KDTree::Node::SetVertexPos(const vec object_center)
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

void KDTree::Node::SetVertexPos(const vec& min, const vec& max)
{
	if (true)
	{
		if (bounding_box.maxPoint.x < max.x)
			bounding_box.maxPoint.x = max.x;

		if (bounding_box.minPoint.x > min.x)
			bounding_box.minPoint.x = min.x;

		if (bounding_box.maxPoint.y < max.y)
			bounding_box.maxPoint.y = max.y;

		if (bounding_box.minPoint.y > min.y)
			bounding_box.minPoint.y = min.y;

		if (bounding_box.maxPoint.z < max.z)
			bounding_box.maxPoint.z = max.z;

		if (bounding_box.minPoint.z > min.z)
			bounding_box.minPoint.z = min.z;

	}
}

float KDTree::Node::GetKdTreeCut(Axis axis)
{
	vec cut = vec::zero;

	for (int i = 0; i < this->objects_in_node.size(); i++)
	{
		cut += this->objects_in_node[i]->getObjectCenter();
	}

	cut = cut / this->objects_in_node.size();

	switch (axis)
	{
	case Axis::Axis_X:
		return cut.x;

	case Axis::Axis_Y:
		return cut.y;

	case Axis::Axis_Z:
		return cut.z;
	}
}
