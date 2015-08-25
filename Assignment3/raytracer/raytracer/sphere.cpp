#include "sphere.h"


int phi;
int theta;
bool gouraud_flag;

Sphere::Sphere(Vec3f point, float r, Material *m)
{
	center_point = point;
	radius = r;
	material = m;
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
			glVertex3f(x0, y0, z0);
			glVertex3f(x1, y1, z1);
			glVertex3f(x2, y2, z2);
			glVertex3f(x3, y3, z3);
			glNormal3f(normal.x(), normal.y(), normal.z());
		}
		
	}
	glEnd();
	//glFlush();
}