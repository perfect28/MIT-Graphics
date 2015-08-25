#include "group.h"

Group::Group(int num)
{
	this->num = num;
	objects = new Object3D*[num];
	boundingBox = new BoundingBox(Vec3f(1e6, 1e6, 1e6), Vec3f(-1e6, -1e6, -1e6));
}

Group::~Group()
{
}

/*
You'll need to write the intersect method of Group which loops through all these instances,
calling their intersection methods. The Group constructor should take as input the number
of objects under the group.
*/

bool Group::intersect(const Ray &r, Hit &h, float tmin)
{
	float minn = numeric_limits<float>::max();

	Vec3f init_normal(0, 0, 0);

	Hit tempHit(0.0f, NULL , init_normal);
	Hit currHit(0.0f, NULL , init_normal);

	/*
	With the intersect routine, we are looking for the closest intersection along a Ray,
	parameterized by t. tmin is used to restrict the range of intersection.
	If an intersection is found such that t > tmin and t is less than the value of the
	intersection currently stored in the Hit data structure, Hit is updated as necessary.
	Note that if the new intersection is closer than the previous one, both t and Material
	must be modified. It is important that your intersection routine verifies that t >= tmin.
	tmin depends on the type of camera (see below) and is not modified by the intersection
	routine.
	*/
	for (int i = 0; i < num; i++)
	{
		if (objects[i]->intersect(r, tempHit, tmin))//获取碰撞当前物体的碰撞信息
		{
			float t = tempHit.getT();
			if (t < minn){
				minn = t;
				currHit = tempHit;
			}
		}
	}

	if (minn == numeric_limits<float>::max())
		return false;//没有碰撞到物体
	else
		h = currHit;//碰撞信息
	return true;
}


/*
Similar to its intersection method, a group implements paint() by iterating over all 
its children and calling their paint() methods.
*/
void Group::paint(void)
{
	for (int i = 0; i < num; i++)
		objects[i]->paint();
}

void Group::addObject(int index, Object3D *obj)
{
	objects[index] = obj;
	if (obj->getBoundingBox()!=NULL)
		boundingBox->Extend(obj->getBoundingBox());
}

void Group::insertIntoGrid(Grid *g, Matrix *m)
{
	for (int i = 0; i < num; i++)
		objects[i]->insertIntoGrid(g,m);
}

void Group::transform(BoundingBox *bb, Matrix m)
{
	for (int i = 0; i < num; i++)
		objects[i]->transform(bb,m);
}