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
private:
	Vec3f center, up, dir, hor;
	float size;
};

#endif