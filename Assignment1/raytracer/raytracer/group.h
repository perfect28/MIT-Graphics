#ifndef _Group_H_
#define _Group_H_
/*
stores an array of pointers to Object3D instances
it will be used to store the entire 3D scene

You'll need to write the intersect method of Group which loops through all these instances, 
calling their intersection methods

The Group constructor should take as input the number of objects under the group
*/
#include "object3d.h"

class Group:public Object3D
{
public:
	Group(int num);
	~Group();
	virtual bool intersect(const Ray &r, Hit &h, float tmin);
	void addObject(int index, Object3D *obj);
private:
	//这里很奇怪，交换一下顺序竟然会报不允许使用不完整的类型的错误
	int num;
	Object3D* objects[];
};



#endif