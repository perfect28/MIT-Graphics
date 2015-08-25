#ifndef _CAMERA_H
#define _CAMERA_H

#include "ray.h"

class Camera
{
public:
	Camera();
	~Camera();
	virtual Ray generateRay(Vec2f point) = 0;
	virtual float getTMin() const = 0;

	virtual void glInit(int w, int h) = 0;
	virtual void glPlaceCamera(void) = 0;
	virtual void dollyCamera(float dist) = 0;
	virtual void truckCamera(float dx, float dy) = 0;
	virtual void rotateCamera(float rx, float ry) = 0;
private:

};

/*
An orthographic camera is described by an orthonormal basis (one point and three vectors) 
and an image size (one floating point).

The constructor takes as input the center of the image, the direction vector, 
an up vector, and the image size.

*/
class OrthographicCamera:public Camera
{
public:
	OrthographicCamera(Vec3f center,Vec3f dir,Vec3f up,float size);
	~OrthographicCamera();

	virtual Ray generateRay(Vec2f point);
	virtual float getTMin() const;

	virtual void glInit(int w, int h);
	virtual void glPlaceCamera(void);
	virtual void dollyCamera(float dist);
	virtual void truckCamera(float dx, float dy);
	virtual void rotateCamera(float rx, float ry);
private:
	Vec3f center, up, dir, hor;
	float size;
};


class PerspectiveCamera:public Camera
{
public:
	PerspectiveCamera(Vec3f cer, Vec3f &direction, Vec3f &up, float angle);
	~PerspectiveCamera();

	virtual Ray generateRay(Vec2f point);
	virtual float getTMin() const;

	virtual void glInit(int w, int h);
	virtual void glPlaceCamera(void);
	virtual void dollyCamera(float dist);
	virtual void truckCamera(float dx, float dy);
	virtual void rotateCamera(float rx, float ry);

private:
	Vec3f center, up, dir, hor;
	float angle, dis;
};



#endif