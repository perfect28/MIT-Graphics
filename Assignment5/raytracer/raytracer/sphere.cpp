#include "sphere.h"
#include "grid.h"

int phi;
int theta;
bool gouraud_flag;

Sphere::Sphere(Vec3f point, float r, Material *m)
{
	center_point = point;
	radius = r;
	material = m;
	Vec3f min(point.x() - r, point.y() - r, point.z() - r);
	Vec3f max(point.x() + r, point.y() + r, point.z() + r);
	boundingBox = new BoundingBox(Vec3f(1e6,1e6,1e6),Vec3f(-1e6,-1e6,-1e6));
	boundingBox->Extend(min);
	boundingBox->Extend(max);
}

Sphere::~Sphere()
{
}

bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
{
	//Assume centered at origin(easy to translate)
	Vec3f ori = r.getOrigin() - center_point;

	// calculate our quadratic equation
	float b = 2.0f*r.getDirection().Dot3(ori);
	float c = ori.Dot3(ori) - radius*radius;

	float d_square = b*b - 4.0f*c;
	if (d_square < 0)
		return false; // negative square root
	//discriminant
	float d = sqrt(d_square);

	//two solutions
	float t1 = (-b - d) / 2.0f;
	float t2 = (-b + d) / 2.0f;

	//choose the closest one
	//先选择t1(t-)，后选择t2(t+)
	float t;
	if (t1 >= tmin)
		t = t1;
	else if (t2 >= tmin)
		t = t2;
	else
		return false; // no good, t less than tmin

	//求交点的法向量,单位化
	Vec3f intersection_point = ori + t*r.getDirection();
	Vec3f normal = intersection_point;
	normal.Normalize();
	h.set(t, material,normal, r);
	return true;
}

const GLfloat Pi = 3.1415926536f;
void Sphere::paint(void)
{
	//glClear(GL_COLOR_BUFFER_BIT);
	//Vec3f mc = material->getDiffuseColor();
	//glColor3f(0.0f, 1.0f, 0.0f);
	material->glSetMaterial();
	glBegin(GL_QUADS);
	/*
	Note that theta should vary between 0 and 360 degrees, 
	while phi must vary between -90 and 90 degrees.
	*/
	for (int iPhi = -90; iPhi<= 90; iPhi += 180/phi)
	for (int iTheta = 0; iTheta <=360; iTheta += 360/theta) {
		float first_phi = iPhi*Pi / 180.0f;
		float sec_phi = (iPhi + 180 / phi)*Pi / 180.0f;
		float first_theta = iTheta*Pi / 180.0f;
		float sec_theta = (iTheta + 360 / theta)*Pi / 180.0f;
		// compute appropriate coordinates & normals
		// send gl vertex commands
		
		float z0 = radius*sin(first_phi);
		float x0 = radius*cos(first_phi)*cos(first_theta);
		float y0 = radius*cos(first_phi)*sin(first_theta);
		x0 = x0 + center_point.x();
		y0 = y0 + center_point.y();
		z0 = z0 + center_point.z();
		//glColor3f(1.0f, 0.0f, 0.0f);

		float z1 = radius*sin(first_phi);
		float x1 = radius*cos(first_phi)*cos(sec_theta);
		float y1 = radius*cos(first_phi)*sin(sec_theta);
		x1 = x1 + center_point.x();
		y1 = y1 + center_point.y();
		z1 = z1 + center_point.z();
		//glColor3f(1.0f, 0.0f, 0.0f);
		float z2 = radius*sin(sec_phi);
		float x2 = radius*cos(sec_phi)*cos(sec_theta);
		float y2 = radius*cos(sec_phi)*sin(sec_theta);
		x2 = x2 + center_point.x();
		y2 = y2 + center_point.y();
		z2 = z2 + center_point.z();
		//glColor3f(1.0f, 0.0f, 0.0f);
		
		float z3 = radius*sin(sec_phi);
		float x3 = radius*cos(sec_phi)*cos(first_theta);
		float y3 = radius*cos(sec_phi)*sin(first_theta);
		x3 = x3 + center_point.x();
		y3 = y3 + center_point.y();
		z3 = z3 + center_point.z();
		//glColor3f(1.0f, 0.0f, 0.0f);

		Vec3f normal1(x0 - center_point.x(), y0 - center_point.y(), z0 - center_point.z());
		Vec3f normal2(x1 - center_point.x(), y1 - center_point.y(), z1 - center_point.z());
		Vec3f normal3(x2 - center_point.x(), y2 - center_point.y(), z2 - center_point.z());
		Vec3f normal4(x3 - center_point.x(), y3 - center_point.y(), z3 - center_point.z());

		Vec3f a(x0 - x1, y0 - y1, z0 - z1);
		Vec3f b(x2 - x1, y2 - y1, z2 - z1);
		Vec3f normal;
		Vec3f::Cross3(normal, b, a);
		
		if (gouraud_flag){
			glNormal3f(normal1.x(), normal1.y(), normal1.z());
			glVertex3f(x0, y0, z0);
			glNormal3f(normal2.x(), normal2.y(), normal2.z());
			glVertex3f(x1, y1, z1);
			glNormal3f(normal3.x(), normal3.y(), normal3.z());
			glVertex3f(x2, y2, z2);
			glNormal3f(normal4.x(), normal4.y(), normal4.z());
			glVertex3f(x3, y3, z3);
		}
		else{
			glNormal3f(normal.x(), normal.y(), normal.z());
			glVertex3f(x0, y0, z0);
			glVertex3f(x1, y1, z1);
			glVertex3f(x2, y2, z2);
			glVertex3f(x3, y3, z3);
		}
	}
	glEnd();
	//glFlush();
}


//调用该方法把球体塞到网格中
//那么比较的是球体的球心和体素的中心
void Sphere::insertIntoGrid(Grid *g, Matrix *m)
{
	Vec3f now_center = center_point;
	if (m != NULL)
	{
		BoundingBox *bb = new BoundingBox(Vec3f(1e6, 1e6, 1e6), Vec3f(-1e6, -1e6, -1e6));
		transform(bb, *m);
		boundingBox = bb;

		//移动圆心
		//发现只是移动圆心并没有用，如果有伸缩变换的话并不能这么做
		//m->Transform(now_center);
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
		//先求出体素的各点坐标
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

		if (m == NULL)
		{
			//注意！！这里的圆心并没有移动过！！！
			Vec3f center = (now_max + now_min);
			center.Divide(2.0f, 2.0f, 2.0f);
			float len = (now_max - center).Length();

			float dis = (center - now_center).Length();
			if (dis < radius + len)
				objects[i*ny*nz + j*nz + k].addObject(this);
		}
		else
		{
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
	}
	g->setObjects(objects);


	//for (int i = 0; i < nx; i++)
	//for (int j = 0; j < ny; j++)
	//for (int k = 0; k < nz; k++)
	//if (objects[i*ny*nz + j*nz + k])
	//	printf("%d %d %d\n", i, j, k);
}

void Sphere::transform(BoundingBox *bb, Matrix m)
{
	Vec3f min, max;
	min = getBoundingBox()->getMin();
	max = getBoundingBox()->getMax();
	Vec3f v1(min.x(), min.y(), min.z());
	Vec3f v2(min.x(), min.y(), max.z());
	Vec3f v3(min.x(), max.y(), min.z());
	Vec3f v4(min.x(), max.y(), max.z());
	Vec3f v5(max.x(), min.y(), min.z());
	Vec3f v6(max.x(), min.y(), max.z());
	Vec3f v7(max.x(), max.y(), min.z());
	Vec3f v8(max.x(), max.y(), max.z());

	m.Transform(v1);
	m.Transform(v2);
	m.Transform(v3);
	m.Transform(v4);
	m.Transform(v5);
	m.Transform(v6);
	m.Transform(v7);
	m.Transform(v8);

	bb->Extend(v1);
	bb->Extend(v2);
	bb->Extend(v3);
	bb->Extend(v4);
	bb->Extend(v5);
	bb->Extend(v6);
	bb->Extend(v7);
	bb->Extend(v8);
}