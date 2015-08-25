
#include "object3d.h"

Object3D::Object3D()
{

}

Object3D::~Object3D()
{

}

BoundingBox* Object3D::getBoundingBox()
{
	return boundingBox;
}

void Object3D::insertIntoGrid(Grid *g, Matrix *m)
{
	//do nothing
}

void Object3D::transform(BoundingBox *bb, Matrix m)
{
	//do nothing
}