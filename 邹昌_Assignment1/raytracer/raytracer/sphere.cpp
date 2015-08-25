#include "sphere.h"

Sphere::Sphere(Vec3f point, float r, Material *m)
{
	center_point = point;
	radius = r;
	material = m;
}

Sphere::~Sphere()
{
}

bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
{
	//Assume centered at origin(easy to translate)
	Vec3f ori = r.getOrigin() - center_point;

	// calculate our quadratic equation
	float b = 2.0f*r.getDirection().Dot3(ori);
	float c = ori.Dot3(ori) - radius*radius;

	float d_square = b*b - 4.0f*c;
	if (d_square < 0)
		return false; // negative square root
	//discriminant
	float d = sqrt(d_square);

	//two solutions
	float t1 = (-b - d) / 2.0f;
	float t2 = (-b + d) / 2.0f;

	//choose the closest one
	//先选择t1(t-)，后选择t2(t+)
	float t;
	if (t1 >= tmin)
		t = t1;
	else if (t2 >= tmin)
		t = t2;
	else
		return false; // no good, t less than tmin

	h.set(t, material, r);
	return true;
}