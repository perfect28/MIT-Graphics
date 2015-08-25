#include "triangle.h"
#include "matrix.h"
#include "grid.h"

Triangle::Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m)
{
	this->a = a;
	this->b = b;
	this->c = c;
	this->material = m;
	boundingBox = new BoundingBox(Vec3f(1e6, 1e6, 1e6), Vec3f(-1e6, -1e6, -1e6));
	boundingBox->Extend(a);
	boundingBox->Extend(b);
	boundingBox->Extend(c);
}

Triangle::~Triangle()
{
}

/*

1.general polygon with in-polygon test
多边形内部测试，用改进的弧长法
2.barycentric coordinates
重心坐标方法

Use the method of your choice to implement the ray-triangle intersection: general 
polygon with in-polygon test, barycentric coordinates, etc. We can compute the normal 
by taking the cross-product of two edges, but note that the normal direction for a 
triangle is ambiguous. We'll use the usual convention that counter-clockwise vertex 
ordering indicates the outward-facing side. If your renderings look incorrect, just 
flip the cross-product to match the convention.

三角形重心坐标法
P = (1-belta-lamda)*a+belta*b+lamda*c;

*/

bool Triangle::intersect(const Ray &r, Hit &h, float tmin)
{
	Vec3f r0 = r.getOrigin();
	Vec3f rd = r.getDirection();

	Vec3f E1 = a - b;
	Vec3f E2 = a - c;
	Vec3f S = a - r0;

	//参数写错，rd写成r0了……

	float de = det3x3(rd.x(), rd.y(), rd.z(), E1.x(), E1.y(), E1.z(), E2.x(), E2.y(), E2.z());
	if (de == 0.0f)
		return false;
	float t = det3x3(S.x(), S.y(), S.z(), E1.x(), E1.y(), E1.z(), E2.x(), E2.y(), E2.z())/de;
	float belta = det3x3(rd.x(), rd.y(), rd.z(), S.x(), S.y(), S.z(), E2.x(), E2.y(), E2.z()) / de;
	float lamda = det3x3(rd.x(), rd.y(), rd.z(), E1.x(), E1.y(), E1.z(), S.x(), S.y(), S.z()) / de;

	Vec3f normal;
	Vec3f::Cross3(normal, b - a, c - a);
	normal.Normalize();
	h.set(t, material, normal, r);
	if (t >= tmin && belta > 0.0f && lamda > 0.0f && belta + lamda < 1.0f)
		return true;
	else
		return false;
}

void Triangle::paint(void)
{
	material->glSetMaterial();
	glBegin(GL_TRIANGLES);
	Vec3f normal;
	Vec3f::Cross3(normal, b - a, c - a);
	normal.Normalize();
	
	Vec3f diffuseColor = material->getDiffuseColor();

	//glColor3f(diffuseColor.x(), diffuseColor.y(), diffuseColor.z());
	glNormal3f(normal.x(), normal.y(), normal.z());
	glVertex3f(a.x(), a.y(), a.z());
	glVertex3f(b.x(), b.y(), b.z());
	glVertex3f(c.x(), c.y(), c.z());
	
	glEnd();
}

void Triangle::insertIntoGrid(Grid *g, Matrix *m)
{
	if (m != NULL){
		boundingBox->Set(Vec3f(1e6, 1e6, 1e6), Vec3f(-1e6, -1e6, -1e6));
		Vec3f v1 = a;
		Vec3f v2 = b;
		Vec3f v3 = c;

		m->Transform(v1);
		m->Transform(v2);
		m->Transform(v3);

		boundingBox->Extend(v1);
		boundingBox->Extend(v2);
		boundingBox->Extend(v3);
	}
	int nx, ny, nz;
	g->getSize(nx, ny, nz);
	Object3DVector* objects = g->getObjects();
	Vec3f min = g->getBoundingBox()->getMin();
	Vec3f max = g->getBoundingBox()->getMax();
	for (int i = 0; i < nx; i++)
	for (int j = 0; j < ny; j++)
	for (int k = 0; k < nz; k++)
	{
		//对于三角形来说,判断它的boundingBox和体素是否相交即可
		Vec3f tmp_min = min;
		Vec3f tmp_max = max;
		tmp_min.Scale(1 - i*1.0 / nx, 1 - j*1.0 / ny, 1 - k*1.0 / nz);
		tmp_max.Scale(i*1.0 / nx, j*1.0 / ny, k*1.0 / nz);
		Vec3f now_min = tmp_min + tmp_max;

		tmp_min = min;
		tmp_max = max;
		tmp_min.Scale(1 - (i + 1)*1.0 / nx, 1 - (j + 1)*1.0 / ny, 1 - (k + 1)*1.0 / nz);
		tmp_max.Scale((i + 1)*1.0 / nx, (j + 1)*1.0 / ny, (k + 1)*1.0 / nz);
		Vec3f now_max = tmp_min + tmp_max;

		tmp_min = getBoundingBox()->getMin();
		tmp_max = getBoundingBox()->getMax();

		//两种情况，一种是完全包含，另一种是部分相交
		
		//情形1：
		if (tmp_min < now_min && now_max < tmp_max ||
			now_min < tmp_min && tmp_max < now_max)
		{
			objects[i*ny*nz + j*nz + k].addObject(this);
		}
		//情形2.1：
		if (tmp_min > now_min && tmp_min < now_max ||
			now_min > tmp_min && now_min < tmp_max)
		{
			objects[i*ny*nz + j*nz + k].addObject(this);
		}
		//情形2.2：

		//这是对于夹在z轴上的情况
		if (tmp_min.x() <= now_min.x() && tmp_min.y() <= now_min.y() &&
			tmp_max.x() >= now_min.x() && tmp_max.y() >= now_min.y() &&
			tmp_min.z() >= now_min.z() && tmp_max.z() <= now_max.z())
		{
			objects[i*ny*nz + j*nz + k].addObject(this);
		}
		if (tmp_min.x() <= now_max.x() && tmp_min.y() <= now_max.y() &&
			tmp_max.x() >= now_max.x() && tmp_max.y() >= now_max.y() &&
			tmp_min.z() >= now_min.z() && tmp_max.z() <= now_min.z())
		{
			objects[i*ny*nz + j*nz + k].addObject(this);
		}

		//这是对于夹在x轴上的情况
		if (tmp_min.z() <= now_min.z() && tmp_min.y() <= now_min.y() &&
			tmp_max.z() >= now_min.z() && tmp_max.y() >= now_min.y() &&
			tmp_min.x() >= now_min.x() && tmp_max.x() <= now_max.x())
		{
			objects[i*ny*nz + j*nz + k].addObject(this);
		}
		if (tmp_min.z() <= now_max.z() && tmp_min.y() <= now_max.y() &&
			tmp_max.z() >= now_max.z() && tmp_max.y() >= now_max.y() &&
			tmp_min.x() >= now_min.x() && tmp_max.x() <= now_min.x())
		{
			objects[i*ny*nz + j*nz + k].addObject(this);
		}

		//这是对于夹在y轴上的情况
		if (tmp_min.x() <= now_min.x() && tmp_min.z() <= now_min.z() &&
			tmp_max.x() >= now_min.x() && tmp_max.z() >= now_min.z() &&
			tmp_min.y() >= now_min.y() && tmp_max.y() <= now_max.y())
		{
			objects[i*ny*nz + j*nz + k].addObject(this);
		}
		if (tmp_min.x() <= now_max.x() && tmp_min.z() <= now_max.z() &&
			tmp_max.x() >= now_max.x() && tmp_max.z() >= now_max.z() &&
			tmp_min.y() >= now_min.y() && tmp_max.y() <= now_min.y())
		{
			objects[i*ny*nz + j*nz + k].addObject(this);
		}
	}
	g->setObjects(objects);

	//for (int i = 0; i < nx; i++)
	//for (int j = 0; j < ny; j++)
	//for (int k = 0; k < nz; k++)
	//if (objects[i*ny*nz + j*nz + k].getNumObjects()>0)
	//	printf("%d %d %d\n", i, j, k);
}

void Triangle::transform(BoundingBox *bb, Matrix m)
{
	Vec3f v1 = a;
	Vec3f v2 = b;
	Vec3f v3 = c;

	m.Transform(v1);
	m.Transform(v2);
	m.Transform(v3);

	bb->Extend(v1);
	bb->Extend(v2);
	bb->Extend(v3);
}