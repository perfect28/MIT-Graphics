
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