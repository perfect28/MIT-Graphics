
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
		//�����h��������ģ��������޸ģ�
		bool judge = object->intersect(Ray(r0,rd), h, tmin);
		Vec3f normal = h.getNormal();
		//�������֣�normal�ķ���û��������Ȼ�����ȴ�ǶԵ�
		//����֮�󷴶��Ǵ�ģ���
		//����ȷ��normalû�д���ô����֮��Ӧ��normal��
		//����
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