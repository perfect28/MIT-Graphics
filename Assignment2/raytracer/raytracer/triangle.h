#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "vectors.h"
#include "material.h"
#include "object3d.h"

class Triangle:public Object3D
{
public:
	Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m);
	~Triangle();
	virtual bool intersect(const Ray &r, Hit &h, float tmin);
private:
	Vec3f a, b, c;
};


#endif