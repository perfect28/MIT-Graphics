#ifndef _SPHERE_H
#define _SPHERE_H
/*
additionally stores a center point and a radius. 
The Sphere constructor will be given the center, radius, and pointer to a Material instance.
*/

#include "object3d.h"

class  Sphere :public Object3D
{
public:
	Sphere(Vec3f point, float r, Material *m);
	~ Sphere();
	virtual bool intersect(const Ray &r, Hit &h, float tmin);

private:
	Vec3f center_point;
	float radius;
};

#endif
