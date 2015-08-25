
#include "transform.h"
#include "matrix.h"
#include "grid.h"

//The conservative bounding box of a transformed object 
//is computed by transforming the 8 corners of the original 
//bounding box and taking their axis-aligned bounding box
Transform::Transform(Matrix &m, Object3D *o)
{
	matrix = m;
	object = o;
	boundingBox = new BoundingBox(Vec3f(1e6, 1e6, 1e6), Vec3f(-1e6, -1e6, -1e6));

	//这里不适合这么做，因为转换的可能是一个组，那么相当于把组
	//合并到一起显示了，应该直接在insertIntoGrid里面下放到各个primitive里面做
	//这里只需要修改一下matrix
	o->transform(boundingBox, m);//传到各个primitives下修改boundingBox
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
		//这里确定normal没有错，那么就是之后应用normal的问题
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


//这里的写法类似于三角形
//原来m就是为了有层次的转换而传递的参数
void Transform::insertIntoGrid(Grid *g, Matrix *m)
{
	if (m == NULL)
		m = &matrix;
	else
		*m = *m * matrix;

	object->insertIntoGrid(g, m);
	
	Matrix inv;
	matrix.Inverse(inv);
	if (*m != matrix)
	{
		*m = *m * inv;
	}
	else
		m = NULL;

	//int nx, ny, nz;
	//g->getSize(nx, ny, nz);
	//bool* objects = g->getObjects();
	//Vec3f min = g->getBoundingBox()->getMin();
	//Vec3f max = g->getBoundingBox()->getMax();
	//for (int i = 0; i < nx; i++)
	//for (int j = 0; j < ny; j++)
	//for (int k = 0; k < nz; k++)
	//{
	//	//对于三角形来说,判断它的boundingBox和体素是否相交即可
	//	Vec3f tmp_min = min;
	//	Vec3f tmp_max = max;
	//	tmp_min.Scale(1 - i*1.0 / nx, 1 - j*1.0 / ny, 1 - k*1.0 / nz);
	//	tmp_max.Scale(i*1.0 / nx, j*1.0 / ny, k*1.0 / nz);
	//	Vec3f now_min = tmp_min + tmp_max;

	//	tmp_min = min;
	//	tmp_max = max;
	//	tmp_min.Scale(1 - (i + 1)*1.0 / nx, 1 - (j + 1)*1.0 / ny, 1 - (k + 1)*1.0 / nz);
	//	tmp_max.Scale((i + 1)*1.0 / nx, (j + 1)*1.0 / ny, (k + 1)*1.0 / nz);
	//	Vec3f now_max = tmp_min + tmp_max;

	//	tmp_min = getBoundingBox()->getMin();
	//	tmp_max = getBoundingBox()->getMax();

	//	//两种情况，一种是完全包含，另一种是部分相交

	//	//情形1：
	//	if (tmp_min < now_min && now_max < tmp_max ||
	//		now_min < tmp_min && tmp_max < now_max)
	//	{
	//		objects[i*ny*nz + j*nz + k] = true;
	//	}
	//	//情形2.1：
	//	if (tmp_min > now_min && tmp_min < now_max ||
	//		now_min > tmp_min && now_min < tmp_max)
	//	{
	//		objects[i*ny*nz + j*nz + k] = true;
	//	}
	//	//情形2.2：
	//	if (tmp_min.x() <= now_min.x() && tmp_min.y() <= now_min.y() &&
	//		tmp_max.x() >= now_min.x() && tmp_max.y() >= now_min.y() &&
	//		tmp_min.z() >= now_min.z() && tmp_max.z() <= now_max.z())
	//	{
	//		objects[i*ny*nz + j*nz + k] = true;
	//	}
	//	if (tmp_min.x() <= now_max.x() && tmp_min.y() <= now_max.y() &&
	//		tmp_max.x() >= now_max.x() && tmp_max.y() >= now_max.y() &&
	//		tmp_min.z() >= now_min.z() && tmp_max.z() <= now_min.z())
	//	{
	//		objects[i*ny*nz + j*nz + k] = true;
	//	}
	//}
	//g->setObjects(objects);
}

void Transform::transform(BoundingBox *bb, Matrix m)
{
	object->transform(bb, m*matrix);
}