#ifndef _OCTREE_H_
#define _OCTREE_H_

#include <vector>
#include "Globals.h"

class GameObject;

class Octree
{
	class Node
	{
	public:
		enum Axis
		{
			Axis_X,
			Axis_Y,
			Axis_Z,
			Axis_NULL
		};

	public:
		Node(std::vector<GameObject*>& objs_in_node, Node * parent, Octree * root);
		Node(std::vector<GameObject*>& objs_in_node, Node * parent, AABB bounding_box);
		~Node();

		void Update();
		void Draw();
		void CleanUp();

		void SetNodeVertex();

		void CreateNodes();
		std::vector<GameObject*> GetObjectsInNode(AABB& new_bounding_box);

		void SetVertexPos(const vec object_center);
		void SetVertexPos(const vec& min, const vec& max);
	public:
		int id;
		int subdivision;

		Octree * root = nullptr;
		Node * parent = nullptr;
		std::vector<Node*> child;

		Axis axis_to_check = Axis::Axis_NULL;

		std::vector<GameObject*> objects_in_node;
		AABB bounding_box;

	};

public:
	Octree(int elements_per_node, int max_subdivisions);
	~Octree();

	void Update();
	void CleanUp();

	void RecursiveGetStaticObjs(const GameObject * obj);

public:
	int elements_per_node;
	int max_subdivisions;

	Node * base_node = nullptr;
	std::vector<Node*> nodes;
	std::vector<GameObject*> static_objs;
};


#endif
