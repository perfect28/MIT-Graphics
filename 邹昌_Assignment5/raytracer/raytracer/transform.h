#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include "object3d.h"
#include "material.h"
#include "matrix.h"

class Transform:Object3D
{
public:
	Transform(Matrix &m, Object3D *o);
	~Transform();
	virtual bool intersect(const Ray &r, Hit &h, float tmin);
	virtual void paint(void);
	void insertIntoGrid(Grid *g, Matrix *m);
	void transform(BoundingBox *bb, Matrix m);
private:
	Object3D *object;
	Matrix matrix;
};

#endif