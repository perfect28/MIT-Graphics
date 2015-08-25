#include "ifs.h"

IFS::IFS()
{
}

IFS::~IFS()
{
	delete[] matrices;
}

void IFS::input(int num, Matrix *matrices, float *p)
{
	this->num = num; 
	this->matrices = matrices;
	this->p = p;
}

void IFS::work(Image *image, int num_points, int n,int size,char filename[])
{
	Vec3f color(255.0, 255.0,0.0);
	for (int i = 0; i < num_points; i++)
	{
		Vec2f point(rand()*1.0 / RAND_MAX, rand()*1.0 / RAND_MAX);
		for (int j = 0; j < n; j++)
		{
			float pb = rand()*1.0 / RAND_MAX;
			int k;
			for (k = 0; k < num;k++)
				if (p[k]>pb) break;
			matrices[k].Transform(point);
		}

		float x, y;
		point.Get(x, y);
		if (x<size && y <size && x >= 0 && y >= 0)
			image->SetPixel((int)(x*size), (int)(y*size),color);
	}
	//image->SaveTGA(filename);
}