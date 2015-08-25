
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
������ƽ�����
��ֱ�ߵĲ�������P = ori + t*dir
���� ƽ�淽�� n * p + D =0

n(R0+t*Rd)+D=0
t=-(D+n*R0)/n*Rd

hit ���ߵ���Ϣ������
float t;
Material *material;
Vec3f normal;
Vec3f intersectionPoint;
*/
bool Plane::intersect(const Ray &r, Hit &h, float tmin)
{
	/*
	��Ҫ�ж������Ƿ���ƽ��ƽ��
	���t<0˵��ƽ�����ӵ�֮��
	��������������������
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