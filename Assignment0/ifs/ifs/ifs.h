/*
a field to store n, the number of transformations
an array of matrices representing the n transformations
an array of the corresponding probabilities for choosing a transformation
a constructor that creates an IFS
an input method that reads the IFS description
a render method that takes as input an image instance, a number of points and a number of iterations
a destructor that frees the memory of the various arrays (using delete)
*/

#include "image.h"
#include "matrix.h"

class IFS
{
public:
	IFS();
	~IFS();

	void input(int num,Matrix *matrices,float *p);
	void work(Image *image, int num_points, int n, int size, char filename[]);

	int Num() const{
		return this->num;
	}
	Matrix Matrice(int k) const{
		return this->matrices[k];
	}
	float P(int k) const{
		return this->p[k];
	}

	void Set(int num){
		this->num = num;
	}
	void Set(int k, Matrix matrice, float p){
		this->matrices[k] = matrice;
		this->p[k] = p;
	}
private:
	int num;//the number of transformations
	Matrix *matrices;//matrices representing the n transformations
	float *p;//probabilities for choosing a transformation
};

