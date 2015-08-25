#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "vectors.h"
#include "object3d.h"

// ====================================================================
// ====================================================================

class Light {

public:

	// CONSTRUCTOR & DESTRUCTOR
	Light() {}
	virtual ~Light() {}

	// VIRTUAL METHODS
	virtual void getIllumination(const Vec3f &p, Vec3f &dir, Vec3f &col,
		float &distanceToLight) const = 0;
	virtual void glInit(int id) = 0;

};

// ====================================================================
// ====================================================================

class DirectionalLight : public Light {

public:

	// CONSTRUCTOR & DESTRUCTOR
	DirectionalLight(const Vec3f &d, const Vec3f &c) {
		direction = d; direction.Normalize();
		color = c;
	}
	~DirectionalLight() {}

	// VIRTUAL METHODS
	void getIllumination(const Vec3f &p, Vec3f &dir, Vec3f &col,
		float &distanceToLight) const {
		// the direction to the light is the opposite of the
		// direction of the directional light source
		dir = direction * (-1.0f);
		col = color;
		// the distance is INFINITY
		distanceToLight = INFINITY;
	}

	void glInit(int id);

private:

	DirectionalLight(); //don't use

	// REPRESENTATION
	Vec3f direction;
	Vec3f color;

};

// ====================================================================
// ====================================================================

class PointLight : public Light {

public:

	// CONSTRUCTOR & DESTRUCTOR
	PointLight(const Vec3f &p, const Vec3f &c, float a1, float a2, float a3) {
		position = p;
		color = c;
		attenuation_1 = a1;
		attenuation_2 = a2;
		attenuation_3 = a3;
	}
	~PointLight() {}


	/*
	在OpenGL中，仅仅支持有限数量的光源。使用GL_LIGHT0表示第0号光源，GL_LIGHT1表示第1号光源，依次类推，OpenGL至少会支持8个
	光源，即GL_LIGHT0到GL_LIGHT7。使用glEnable函数可以开启它们。例如，glEnable(GL_LIGHT0);可以开启第0号光源。使用glDisable
	函数则可以关闭光源。一些OpenGL实现可能支持更多数量的光源，但总的来说，开启过多的光源将会导致程序运行速度的严重下降，玩
	过3D Mark的朋友可能多少也有些体会。一些场景中可能有成百上千的电灯，这时可能需要采取一些近似的手段来进行编程，否则以目前
	的计算机而言，是无法运行这样的程序的。
每一个光源都可以设置其属性，这一动作是通过glLight*函数完成的。glLight*函数具有三个参数，第一个参数指明是设置哪一个光源的属性，
第二个参数指明是设置该光源的哪一个属性，第三个参数则是指明把该属性值设置成多少。光源的属性众多，下面将分别介绍。
（1）GL_AMBIENT、GL_DIFFUSE、GL_SPECULAR属性。这三个属性表示了光源所发出的光的反射特性（以及颜色）。每个属性由四个值表示，
分别代表了颜色的R, G, B, A值。GL_AMBIENT表示该光源所发出的光，经过非常多次的反射后，最终遗留在整个光照环境中的强度（颜色）。
GL_DIFFUSE表示该光源所发出的光，照射到粗糙表面时经过漫反射，所得到的光的强度（颜色）。GL_SPECULAR表示该光源所发出的光，照射
到光滑表面时经过镜面反射，所得到的光的强度（颜色）。
（2）GL_POSITION属性。表示光源所在的位置。由四个值（X, Y, Z, W）表示。如果第四个值W为零，则表示该光源位于无限远处，前三个值
表示了它所在的方向。这种光源称为方向性光源，通常，太阳可以近似的被认为是方向性光源。如果第四个值W不为零，则X/W, Y/W, Z/W表示
了光源的位置。这种光源称为位置性光源。对于位置性光源，设置其位置与设置多边形顶点的方式相似，各种矩阵变换函数例如：glTranslate*
、glRotate*等在这里也同样有效。方向性光源在计算时比位置性光源快了不少，因此，在视觉效果允许的情况下，应该尽可能的使用方向性光
源。
（3）GL_SPOT_DIRECTION、GL_SPOT_EXPONENT、GL_SPOT_CUTOFF属性。表示将光源作为聚光灯使用（这些属性只对位置性光源有效）。很多光
源都是向四面八方发射光线，但有时候一些光源则是只向某个方向发射，比如手电筒，只向一个较小的角度发射光线。GL_SPOT_DIRECTION属
性有三个值，表示一个向量，即光源发射的方向。GL_SPOT_EXPONENT属性只有一个值，表示聚光的程度，为零时表示光照范围内向各方向发射
的光线强度相同，为正数时表示光照向中央集中，正对发射方向的位置受到更多光照，其它位置受到较少光照。数值越大，聚光效果就越明显。
GL_SPOT_CUTOFF属性也只有一个值，表示一个角度，它是光源发射光线所覆盖角度的一半（见图2），其取值范围在0到90之间，也可以取180
这个特殊值。取值为180时表示光源发射光线覆盖360度，即不使用聚光灯，向全周围发射。
（4）GL_CONSTANT_ATTENUATION、GL_LINEAR_ATTENUATION、GL_QUADRATIC_ATTENUATION属性。这三个属性表示了光源所发出的光线的直线传
播特性（这些属性只对位置性光源有效）。现实生活中，光线的强度随着距离的增加而减弱，OpenGL把这个减弱的趋势抽象成函数：
衰减因子 = 1 / (k1 + k2 * d + k3 * k3 * d)
其中d表示距离，光线的初始强度乘以衰减因子，就得到对应距离的光线强度。k1, k2, k3分别就是GL_CONSTANT_ATTENUATION, 
GL_LINEAR_ATTENUATION, GL_QUADRATIC_ATTENUATION。通过设置这三个常数，就可以控制光线在传播过程中的减弱趋势。
属性还真是不少。当然了，如果是使用方向性光源，（3）（4）这两类属性就不会用到了，问题就变得简单明了
	*/
	// VIRTUAL METHODS
	void getIllumination(const Vec3f &p, Vec3f &dir, Vec3f &col, float &distanceToLight) const {
		dir = position - p;
		// grab the length before the direction is normalized
		distanceToLight = dir.Length();
		dir.Normalize();
		float attenuation = 1 / (attenuation_1 +
			attenuation_2*distanceToLight +
			attenuation_3*distanceToLight*distanceToLight);
		if (attenuation < 0) attenuation = 0;
		col = color * attenuation;
	}

	void glInit(int id);

private:

	PointLight(); // don't use

	// REPRESENTATION
	Vec3f position;
	Vec3f color;

	// attenuation of the light
	float attenuation_1;
	float attenuation_2;
	float attenuation_3;

};

// ====================================================================
// ====================================================================


#endif