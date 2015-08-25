#ifndef _GRID_H
#define _GRID_H

#include "object3d.h"
#include "marchingInfo.h"
#include "object3dvector.h"

class Grid :public Object3D
{
public:
	Grid(BoundingBox *bb, int nx, int ny, int nz);
	~Grid();

	virtual bool intersect(const Ray &r, Hit &h, float tmin);

	virtual void paint(void);

	BoundingBox* getBoundingBox();
	void getSize(int &x, int &y, int &z);
	Object3DVector* getObjects();
	void setObjects(Object3DVector* objects);

	void initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const;
	void debug(MarchingInfo mi, int i, int j, int k);
private:
	int nx, ny, nz;
	float grid_x, grid_y, grid_z;
	Object3DVector *objects;
};

#endif