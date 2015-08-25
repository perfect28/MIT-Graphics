#ifndef _Object3D_H_
#define _Object3D_H_

#include "hit.h"
#include "material.h"

/*
a default constructor and destructor,
a pointer to a Material instance, and
a pure virtual intersection method.
*/
class Object3D{

public:
	Object3D();
	~Object3D();
	virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
protected:
	Material* material;
};


#endif