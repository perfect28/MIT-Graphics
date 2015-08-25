
#include "plane.h"





//d is the offset from the origin, meaning that the plane equation is P.n = d.
Plane::Plane(Vec3f &normal, float d, Material *m)
{
	this->normal = normal;
	this->normal.Normalize();
	this->d = d;
	this->material = m;
}

Plane::~Plane()
{
}

/*
射线与平面求教
即直线的参数方程P = ori + t*dir
代入 平面方程 n * p + D =0

n(R0+t*Rd)+D=0
t=-(D+n*R0)/n*Rd

hit 带走的信息包括：
float t;
Material *material;
Vec3f normal;
Vec3f intersectionPoint;
*/
bool Plane::intersect(const Ray &r, Hit &h, float tmin)
{
	/*
	需要判断射线是否与平面平行
	如果t<0说明平面在视点之后
	以上两个都不产生交点
	*/
	float eps = 1e-6;
	float tmp = normal.Dot3(r.getDirection());
	if (tmp < eps)
		return false;
	float t = 0.0f;
	Vec3f r0 = r.getOrigin();
	Vec3f rd = r.getDirection();

	t = -(d + normal.Dot3(r0)) / normal.Dot3(rd);
	if (t < 0)
		return false;
	h.set(t, material, normal, r);
	if (t >= tmin)
		return true;
	else
		return false;
}


/*
Plane - OpenGL does not have an infinite plane primitive. To pre-visualize planes, 
you will simply draw very big rectangles. Project the world origin (0,0,0) onto 
the plane, and compute two basis vectors for the plane that are orthogonal to the 
plane normal n. The first basis vector may be obtained by taking the cross product 
between n and another vector v. Any vector v will do the trick, as long as it is 
not parallel to n. So you can always use v=(1,0,0) except when n is along the x axis, 
in which case you can use v=(0,1,0). Then the first basis vector, b1, is v x n and 
the second basis vector, b2, is n x b1. Display a rectangle from (-big, -big) to 
(big, big) in this 2D basis, for some big number big. (Caution: OpenGL does not like 
rendering points at INFINITY. big should probably be < 106)
*/
void Plane::paint(void)
{
	material->glSetMaterial();
	glBegin(GL_QUADS);
	glNormal3f(normal.x(),normal.y(),normal.z());

	Vec3f v(1.0f, 0.0f, 0.0f);
	//向量平行，叉乘的模等于0
	Vec3f res;
	Vec3f::Cross3(res, normal, v);
	if (res.Length() < 1e-6)
		v.Set(0.0f, 1.0f, 0.0f);

	Vec3f b1,b2;
	Vec3f::Cross3(b1, v, normal);
	Vec3f::Cross3(b2, normal, b1);
	glVertex3f(b1.x(), b1.y(), b1.z());
	glVertex3f(-b1.x(), -b1.y(), -b2.z());
	glVertex3f(b2.x(), b2.y(), b1.z());
	glVertex3f(-b2.x(), -b2.y(), -b2.z());
	glEnd();
}