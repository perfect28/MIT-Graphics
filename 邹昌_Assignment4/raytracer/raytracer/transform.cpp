
#include "transform.h"
#include "matrix.h"

Transform::Transform(Matrix &m, Object3D *o)
{
	matrix = m;
	object = o;
}

Transform::~Transform()
{
}

/*
The intersect routine will first transform the ray, 
then delegate to the intersect routine of the contained 
object. Make sure to correctly transform the resulting 
normal according to the rule seen in lecture. You may 
choose to normalize the direction of the transformed ray 
or leave it un-normalized. If you decide not to normalize 
the direction, you might need to update some of your intersection code.
*/
bool Transform::intersect(const Ray &r, Hit &h, float tmin)
{
	Vec3f r0 = r.getOrigin();
	Vec3f rd = r.getDirection();
	Matrix inv;
	matrix.Inverse(inv);
	inv.Transform(r0);
	inv.TransformDirection(rd);
	if (object != NULL)
	{
		//这里的h是有问题的，作如下修改：
		bool judge = object->intersect(Ray(r0,rd), h, tmin);
		Vec3f normal = h.getNormal();
		//这里很奇怪，normal的方向没有修正，然而结果却是对的
		//改了之后反而是错的！！
		//这里确定normal没有错，那么就是之后应用normal的
		//问题
		//好吧，就是这里的问题
		//经过把图形摆正，发现求的法向量没有问题，但是没有单位化…………！
		matrix.TransformDirection(normal);
		normal.Normalize();
		//or：
		//Matrix change,res;
		//matrix.Inverse(change);
		//change.Transpose(res);
		//res.TransformDirection(normal);
		h.set(h.getT(), h.getMaterial(), normal, r);
		return judge;
	}
	return false;
}

void Transform::paint(void)
{
	glPushMatrix();
	GLfloat *glMatrix = matrix.glGet();
	glMultMatrixf(glMatrix);
	delete[] glMatrix;
	object->paint();
	glPopMatrix();
}