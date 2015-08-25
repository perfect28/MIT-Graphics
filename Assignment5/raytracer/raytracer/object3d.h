#ifndef _Object3D_H_
#define _Object3D_H_

#include "hit.h"
#include "material.h"
#include <GL/glut.h>
#include "boundingbox.h"
#include "matrix.h"

/*
a default constructor and destructor,
a pointer to a Material instance, and
a pure virtual intersection method.
*/
class Grid;
class Object3D{

public:
	Object3D();
	~Object3D();
	virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;

	virtual void paint(void) = 0;

	virtual void insertIntoGrid(Grid *g, Matrix *m);
	virtual void transform(BoundingBox *bb, Matrix m);

	BoundingBox* getBoundingBox();
protected:
	Material* material;
	BoundingBox* boundingBox;
};


#endif