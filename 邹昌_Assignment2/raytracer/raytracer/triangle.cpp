#include "triangle.h"
#include "matrix.h"

Triangle::Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m)
{
	this->a = a;
	this->b = b;
	this->c = c;
	this->material = m;
}

Triangle::~Triangle()
{
}


/*

1.general polygon with in-polygon test
多边形内部测试，用改进的弧长法
2.barycentric coordinates
重心坐标方法

Use the method of your choice to implement the ray-triangle intersection: general 
polygon with in-polygon test, barycentric coordinates, etc. We can compute the normal 
by taking the cross-product of two edges, but note that the normal direction for a 
triangle is ambiguous. We'll use the usual convention that counter-clockwise vertex 
ordering indicates the outward-facing side. If your renderings look incorrect, just 
flip the cross-product to match the convention.

三角形重心坐标法
P = (1-belta-lamda)*a+belta*b+lamda*c;

*/

bool Triangle::intersect(const Ray &r, Hit &h, float tmin)
{
	Vec3f r0 = r.getOrigin();
	Vec3f rd = r.getDirection();

	Vec3f E1 = a - b;
	Vec3f E2 = a - c;
	Vec3f S = a - r0;

	//参数写错，rd写成r0了……

	float de = det3x3(rd.x(), rd.y(), rd.z(), E1.x(), E1.y(), E1.z(), E2.x(), E2.y(), E2.z());
	if (de == 0.0f)
		return false;
	float t = det3x3(S.x(), S.y(), S.z(), E1.x(), E1.y(), E1.z(), E2.x(), E2.y(), E2.z())/de;
	float belta = det3x3(rd.x(), rd.y(), rd.z(), S.x(), S.y(), S.z(), E2.x(), E2.y(), E2.z()) / de;
	float lamda = det3x3(rd.x(), rd.y(), rd.z(), E1.x(), E1.y(), E1.z(), S.x(), S.y(), S.z()) / de;

	Vec3f normal;
	Vec3f::Cross3(normal, b - a, c - a);
	normal.Normalize();
	h.set(t, material, normal, r);
	if (t >= tmin && belta > 0.0f && lamda > 0.0f && belta + lamda < 1.0f)
		return true;
	else
		return false;
}