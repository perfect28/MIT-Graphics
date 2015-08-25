#include "grid.h"
#include "material.h"
#include "rayTree.h"

Grid::Grid(BoundingBox *bb, int nx, int ny, int nz)
{
	boundingBox = bb;
	this->nx = nx;
	this->ny = ny;
	this->nz = nz;
	Vec3f min = bb->getMin();
	Vec3f max = bb->getMax();
	this->grid_x = (max.x() - min.x()) / nx;
	this->grid_y = (max.y() - min.y()) / ny;
	this->grid_z = (max.z() - min.z()) / nz;
	objects = new Object3DVector[nx*ny*nz];
	//for (int i = 0; i < nx*ny*nz; i++)
	//	objects[i] = false; 
}

Grid::~Grid()
{
	delete[] objects;
}

void swap(float &u, float &v)
{
	float tmp;
	tmp = u;
	u = v;
	v = tmp;
}

float min_f(float x, float y, float z)
{
	float tmp = x;
	if (tmp > y) tmp = y;
	if (tmp > z) tmp = z;
	return tmp;
}

float max_f(float x, float y, float z)
{
	float tmp = x;
	if (tmp < y) tmp = y;
	if (tmp < z) tmp = z;
	return tmp;
}

bool Grid::intersect(const Ray &r, Hit &h, float tmin)
{
	MarchingInfo mi;
	Vec3f diffuseColor(1, 1, 1);
	Vec3f specularColor(0, 0, 0);
	float exponent = 1;
	Vec3f reflectiveColor(0, 0, 0);
	Vec3f transparentColor(0, 0, 0);
	float indexOfRefraction = 1;
	Material *m = new PhongMaterial(diffuseColor, specularColor, exponent,
		reflectiveColor, transparentColor, indexOfRefraction);

	initializeRayMarch(mi, r, tmin);
	
	int i, j, k;
	mi.getIndices(i, j, k);
	debug(mi, i, j, k);
	

	while (i >= 0 && i < nx && j >= 0 && j < ny && k >= 0 && k < nz)
	{
		int num = objects[i*ny*nz + j*nz + k].getNumObjects();
		if (num > 0){
			h.set(mi.get_tmin(), m, mi.getNormal(), r);
			return true;
		}
		//这里是下一节的内容
		//for (int nn = 0; nn < num; nn++)
		//{
		//	Object3D *o = objects[i*ny*nz + j*nz + k].getObject(nn);
		//	if (o->intersect(r, h, tmin))
		//	{
		//		//这里还需要比较得到的t值是否就是在当前这个cell内
		//		return true;
		//	}
		//}
		mi.nextCell();
		mi.getIndices(i, j, k);
		debug(mi, i, j, k);
	}

	h.set(mi.get_tmin(), m, mi.getNormal(), r);
	if (i >= 0 && i < nx && j >= 0 && j < ny && k >= 0 && k < nz)
		return true;
	else
		return false;
}

void Grid::debug(MarchingInfo mi,int i, int j, int k)
{
	Vec3f diffuseColor(1, 1, 1);
	Vec3f specularColor(0, 0, 0);
	float exponent = 1;
	Vec3f reflectiveColor(0, 0, 0);
	Vec3f transparentColor(0, 0, 0);
	float indexOfRefraction = 1;
	Material *m = new PhongMaterial(diffuseColor, specularColor, exponent,
		reflectiveColor, transparentColor, indexOfRefraction);

	Vec3f min = boundingBox->getMin();
	Vec3f max = boundingBox->getMax();
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

	Vec3f v1(now_min.x(), now_min.y(), now_min.z());
	Vec3f v2(now_min.x(), now_min.y(), now_max.z());
	Vec3f v3(now_min.x(), now_max.y(), now_min.z());
	Vec3f v4(now_min.x(), now_max.y(), now_max.z());
	Vec3f v5(now_max.x(), now_min.y(), now_min.z());
	Vec3f v6(now_max.x(), now_min.y(), now_max.z());
	Vec3f v7(now_max.x(), now_max.y(), now_min.z());
	Vec3f v8(now_max.x(), now_max.y(), now_max.z());

	Vec3f a, b, c, d;
	if (mi.getNormal() == Vec3f(1.0f, 0.0f, 0.0f))
	{
		a = v5; b = v6; c = v8; d = v7;
	}
	else if (mi.getNormal() == Vec3f(-1.0f, 0.0f, 0.0f))
	{
		a = v1; b = v2; c = v4; d = v3;
	}
	else if (mi.getNormal() == Vec3f(0.0f, 1.0f, 0.0f))
	{
		a = v3; b = v4; c = v8; d = v7;
	}
	else if (mi.getNormal() == Vec3f(0.0f, -1.0f, 0.0f))
	{
		a = v1; b = v2; c = v6; d = v5;
	}
	else if (mi.getNormal() == Vec3f(0.0f, 0.0f, 1.0f))
	{
		a = v4; b = v2; c = v6; d = v8;
	}
	else if (mi.getNormal() == Vec3f(0.0f, 0.0f, -1.0f))
	{
		a = v1; b = v5; c = v7; d = v3;
	}

	RayTree::AddHitCellFace(a, b, c, d, mi.getNormal(), m);
	RayTree::AddEnteredFace(v5, v6, v8, v7, mi.getNormal(), m);
	RayTree::AddEnteredFace(v1, v2, v4, v3, mi.getNormal(), m);
	RayTree::AddEnteredFace(v3, v4, v8, v7, mi.getNormal(), m);
	RayTree::AddEnteredFace(v1, v2, v6, v5, mi.getNormal(), m);
	RayTree::AddEnteredFace(v4, v2, v6, v8, mi.getNormal(), m);
	RayTree::AddEnteredFace(v1, v5, v7, v3, mi.getNormal(), m);
}


void Grid::initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const
{
	Vec3f r0 = r.getOrigin();
	Vec3f rd = r.getDirection();
	Vec3f min = boundingBox->getMin();
	Vec3f max = boundingBox->getMax();
	/*初始化各个变量*/

	//初始化sign
	int sign_x = 0, sign_y = 0, sign_z = 0;
	if (rd.x() != 0) sign_x = rd.x() > 0 ? 1 : -1;
	if (rd.y() != 0) sign_y = rd.y() > 0 ? 1 : -1;
	if (rd.z() != 0) sign_z = rd.z() > 0 ? 1 : -1;
	mi.setSign(sign_x, sign_y, sign_z);

	//初始化delta
	//这里的计算还存在问题，网格的宽度并不是1
	//而且实际上rd.Length()==1
	float dt_x, dt_y, dt_z;
	//dt_x = rd.Length() / rd.x();
	//dt_y = rd.Length() / rd.y();
	//dt_z = rd.Length() / rd.z();
	dt_x = fabs(grid_x / rd.x());
	dt_y = fabs(grid_y / rd.y());
	dt_z = fabs(grid_z / rd.z());
	mi.setDelta(dt_x, dt_y, dt_z);

	//初始化next

	/*
	三种相交情况：
	当原点在网格内
	当原点在网格外，光线与网格相交
	当原点在网格外，光线与网格不交
	*/
	//mi：存放的是当前grid信息

	//ray-box intersection
	float tx1, tx2;
	float ty1, ty2;
	float tz1, tz2;
	float tanswer = 1e6;
	for (int i = 0; i < nx; i++)
	for (int j = 0; j < ny; j++)
	for (int k = 0; k < nz; k++)
	if (i == 0 || i == nx - 1 || j == 0 || j == ny - 1 || k == 0 || k == nz - 1)
	{
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

		tx1 = (now_min.x() - r0.x()) / rd.x();
		tx2 = (now_max.x() - r0.x()) / rd.x();
		if (tx1 > tx2)
			swap(tx1, tx2);

		ty1 = (now_min.y() - r0.y()) / rd.y();
		ty2 = (now_max.y() - r0.y()) / rd.y();
		if (ty1 > ty2)
			swap(ty1, ty2);

		tz1 = (now_min.z() - r0.z()) / rd.z();
		tz2 = (now_max.z() - r0.z()) / rd.z();
		if (tz1 > tz2)
			swap(tz1, tz2);

		float t_near = max_f(tx1, ty1, tz1);
		float t_far = min_f(tx2, ty2, tz2);

		if (t_near < t_far && t_far > tmin)
		{
			float t_next_x, t_next_y, t_next_z;
			if (t_near > tmin && t_near<tanswer)
			{
				mi.setIndices(i, j, k);//找到了网格的下标
				mi.set_tmin(t_near);
				t_next_x = tx2;
				t_next_y = ty2;
				t_next_z = tz2;
				mi.setNext(t_next_x, t_next_y, t_next_z);

				Vec3f normal;

				if (t_near == tx1) {
					if (rd.x() > 0) normal.Set(-1.0f, 0.0f, 0.0f);
					else normal.Set(1.0f, 0.0f, 0.0f);
				}
				if (t_near == ty1) {
					if (rd.y() > 0) normal.Set(0.0f, -1.0f, 0.0f);
					else normal.Set(0.0f, 1.0f, 0.0f);
				}
				if (t_near == tz1) {
					if (rd.z() > 0) normal.Set(0.0f, 0.0f, -1.0f);
					else normal.Set(0.0f, 0.0f, 1.0f);
				}

				mi.setNormal(normal);
			}
			else if(t_near <= tmin && t_far<tanswer)//视点在网格内
			{ 
				mi.setIndices(i, j, k);//找到了网格的下标
				mi.set_tmin(0.0f);
				t_next_x = tx2;
				t_next_y = ty2;
				t_next_z = tz2;
				mi.setNext(t_next_x, t_next_y, t_next_z);

				Vec3f normal;

				if (t_far == tx2) {
					if (rd.x() > 0) normal.Set(1.0f, 0.0f, 0.0f);
					else normal.Set(-1.0f, 0.0f, 0.0f);
				}
				if (t_far == ty2) {
					if (rd.y() > 0) normal.Set(0.0f, 1.0f, 0.0f);
					else normal.Set(0.0f, -1.0f, 0.0f);
				}
				if (t_far == tz2) {
					if (rd.z() > 0) normal.Set(0.0f, 0.0f, 1.0f);
					else normal.Set(0.0f, 0.0f, -1.0f);
				}

				mi.setNormal(normal);
			}
		}
	}
}



void Grid::paint(void)
{
	for (int i = 0; i < nx; i++)
	for (int j = 0; j < ny; j++)
	for (int k = 0; k < nz; k++)
	if (objects[i*ny*nz + j*nz + k].getNumObjects()>0)
		printf("%d\n", i*ny*nz + j*nz + k);

	for (int i = 0; i < nx; i++)
	for (int j = 0; j < ny; j++)
	for (int k = 0; k < nz; k++)
	{
		if (objects[i*ny*nz + j*nz + k].getNumObjects()>0)//表示被覆盖了
		{
			//先求出体素的各点坐标
			Vec3f min = boundingBox->getMin();
			Vec3f max = boundingBox->getMax();

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

			Vec3f v1(now_min.x(), now_min.y(), now_min.z());
			Vec3f v2(now_min.x(), now_min.y(), now_max.z());
			Vec3f v3(now_min.x(), now_max.y(), now_min.z());
			Vec3f v4(now_min.x(), now_max.y(), now_max.z());
			Vec3f v5(now_max.x(), now_min.y(), now_min.z());
			Vec3f v6(now_max.x(), now_min.y(), now_max.z());
			Vec3f v7(now_max.x(), now_max.y(), now_min.z());
			Vec3f v8(now_max.x(), now_max.y(), now_max.z());

			//v1.print();
			//v2.print();
			//v3.print();
			//v4.print();
			//v5.print();
			//v6.print();
			//v7.print();
			//v8.print();

			//material->glSetMaterial();
			glBegin(GL_QUADS);
			Vec3f normal;
			
			//左面
			Vec3f::Cross3(normal, v1 - v2, v1 - v3);
			normal.Normalize();
			glNormal3f(normal.x(), normal.y(), normal.z());
			glVertex3f(v1.x(), v1.y(), v1.z());
			glVertex3f(v2.x(), v2.y(), v2.z());
			glVertex3f(v4.x(), v4.y(), v4.z());
			glVertex3f(v3.x(), v3.y(), v3.z());

			//右面
			Vec3f::Cross3(normal, v5 - v7, v5 - v6);
			normal.Normalize();
			glNormal3f(normal.x(), normal.y(), normal.z());
			glVertex3f(v5.x(), v5.y(), v5.z());
			glVertex3f(v6.x(), v6.y(), v6.z());
			glVertex3f(v8.x(), v8.y(), v8.z());
			glVertex3f(v7.x(), v7.y(), v7.z());

			//上面
			Vec3f::Cross3(normal, v3 - v4, v3 - v7);
			normal.Normalize();
			glNormal3f(normal.x(), normal.y(), normal.z());
			glVertex3f(v3.x(), v3.y(), v3.z());
			glVertex3f(v4.x(), v4.y(), v4.z());
			glVertex3f(v8.x(), v8.y(), v8.z());
			glVertex3f(v7.x(), v7.y(), v7.z());

			//下面
			Vec3f::Cross3(normal, v1 - v5, v1 - v2);
			normal.Normalize();
			glNormal3f(normal.x(), normal.y(), normal.z());
			glVertex3f(v1.x(), v1.y(), v1.z());
			glVertex3f(v2.x(), v2.y(), v2.z());
			glVertex3f(v6.x(), v6.y(), v6.z());
			glVertex3f(v5.x(), v5.y(), v5.z());

			//前面
			Vec3f::Cross3(normal, v2 - v6, v2 - v4);
			normal.Normalize();
			glNormal3f(normal.x(), normal.y(), normal.z());
			glVertex3f(v2.x(), v2.y(), v2.z());
			glVertex3f(v6.x(), v6.y(), v6.z());
			glVertex3f(v8.x(), v8.y(), v8.z());
			glVertex3f(v4.x(), v4.y(), v4.z());

			//后面
			Vec3f::Cross3(normal, v1 - v3, v1 - v5);
			normal.Normalize();
			glNormal3f(normal.x(), normal.y(), normal.z());
			glVertex3f(v1.x(), v1.y(), v1.z());
			glVertex3f(v5.x(), v5.y(), v5.z());
			glVertex3f(v7.x(), v7.y(), v7.z());
			glVertex3f(v3.x(), v3.y(), v3.z());

			glEnd();
		}
	}

}

BoundingBox* Grid::getBoundingBox()
{
	return boundingBox;
}

void Grid::getSize(int &x, int &y, int &z)
{
	x = nx;
	y = ny;
	z = nz;
}

Object3DVector* Grid::getObjects()
{
	return objects;
}

void Grid::setObjects(Object3DVector* objects)
{
	this->objects = objects;
}

