
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

	//���ﲻ�ʺ���ô������Ϊת���Ŀ�����һ���飬��ô�൱�ڰ���
	//�ϲ���һ����ʾ�ˣ�Ӧ��ֱ����insertIntoGrid�����·ŵ�����primitive������
	//����ֻ��Ҫ�޸�һ��matrix
	o->transform(boundingBox, m);//��������primitives���޸�boundingBox
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
		//�����h��������ģ��������޸ģ�
		bool judge = object->intersect(Ray(r0,rd), h, tmin);
		Vec3f normal = h.getNormal();
		//�������֣�normal�ķ���û��������Ȼ�����ȴ�ǶԵ�
		//����֮�󷴶��Ǵ�ģ���
		//����ȷ��normalû�д���ô����֮��Ӧ��normal������
		//�ðɣ��������������
		//������ͼ�ΰ�����������ķ�����û�����⣬����û�е�λ������������
		matrix.TransformDirection(normal);
		normal.Normalize();
		//or��
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


//�����д��������������
//ԭ��m����Ϊ���в�ε�ת�������ݵĲ���
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
	//	//������������˵,�ж�����boundingBox�������Ƿ��ཻ����
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

	//	//���������һ������ȫ��������һ���ǲ����ཻ

	//	//����1��
	//	if (tmp_min < now_min && now_max < tmp_max ||
	//		now_min < tmp_min && tmp_max < now_max)
	//	{
	//		objects[i*ny*nz + j*nz + k] = true;
	//	}
	//	//����2.1��
	//	if (tmp_min > now_min && tmp_min < now_max ||
	//		now_min > tmp_min && now_min < tmp_max)
	//	{
	//		objects[i*ny*nz + j*nz + k] = true;
	//	}
	//	//����2.2��
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