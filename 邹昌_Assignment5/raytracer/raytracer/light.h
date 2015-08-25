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
	��OpenGL�У�����֧�����������Ĺ�Դ��ʹ��GL_LIGHT0��ʾ��0�Ź�Դ��GL_LIGHT1��ʾ��1�Ź�Դ���������ƣ�OpenGL���ٻ�֧��8��
	��Դ����GL_LIGHT0��GL_LIGHT7��ʹ��glEnable�������Կ������ǡ����磬glEnable(GL_LIGHT0);���Կ�����0�Ź�Դ��ʹ��glDisable
	��������Թرչ�Դ��һЩOpenGLʵ�ֿ���֧�ָ��������Ĺ�Դ�����ܵ���˵����������Ĺ�Դ���ᵼ�³��������ٶȵ������½�����
	��3D Mark�����ѿ��ܶ���Ҳ��Щ��ᡣһЩ�����п����гɰ���ǧ�ĵ�ƣ���ʱ������Ҫ��ȡһЩ���Ƶ��ֶ������б�̣�������Ŀǰ
	�ļ�������ԣ����޷����������ĳ���ġ�
ÿһ����Դ���������������ԣ���һ������ͨ��glLight*������ɵġ�glLight*��������������������һ������ָ����������һ����Դ�����ԣ�
�ڶ�������ָ�������øù�Դ����һ�����ԣ���������������ָ���Ѹ�����ֵ���óɶ��١���Դ�������ڶ࣬���潫�ֱ���ܡ�
��1��GL_AMBIENT��GL_DIFFUSE��GL_SPECULAR���ԡ����������Ա�ʾ�˹�Դ�������Ĺ�ķ������ԣ��Լ���ɫ����ÿ���������ĸ�ֵ��ʾ��
�ֱ��������ɫ��R, G, B, Aֵ��GL_AMBIENT��ʾ�ù�Դ�������Ĺ⣬�����ǳ���εķ���������������������ջ����е�ǿ�ȣ���ɫ����
GL_DIFFUSE��ʾ�ù�Դ�������Ĺ⣬���䵽�ֲڱ���ʱ���������䣬���õ��Ĺ��ǿ�ȣ���ɫ����GL_SPECULAR��ʾ�ù�Դ�������Ĺ⣬����
���⻬����ʱ�������淴�䣬���õ��Ĺ��ǿ�ȣ���ɫ����
��2��GL_POSITION���ԡ���ʾ��Դ���ڵ�λ�á����ĸ�ֵ��X, Y, Z, W����ʾ��������ĸ�ֵWΪ�㣬���ʾ�ù�Դλ������Զ����ǰ����ֵ
��ʾ�������ڵķ������ֹ�Դ��Ϊ�����Թ�Դ��ͨ����̫�����Խ��Ƶı���Ϊ�Ƿ����Թ�Դ��������ĸ�ֵW��Ϊ�㣬��X/W, Y/W, Z/W��ʾ
�˹�Դ��λ�á����ֹ�Դ��Ϊλ���Թ�Դ������λ���Թ�Դ��������λ�������ö���ζ���ķ�ʽ���ƣ����־���任�������磺glTranslate*
��glRotate*��������Ҳͬ����Ч�������Թ�Դ�ڼ���ʱ��λ���Թ�Դ���˲��٣���ˣ����Ӿ�Ч�����������£�Ӧ�þ����ܵ�ʹ�÷����Թ�
Դ��
��3��GL_SPOT_DIRECTION��GL_SPOT_EXPONENT��GL_SPOT_CUTOFF���ԡ���ʾ����Դ��Ϊ�۹��ʹ�ã���Щ����ֻ��λ���Թ�Դ��Ч�����ܶ��
Դ����������˷�������ߣ�����ʱ��һЩ��Դ����ֻ��ĳ�������䣬�����ֵ�Ͳ��ֻ��һ����С�ĽǶȷ�����ߡ�GL_SPOT_DIRECTION��
��������ֵ����ʾһ������������Դ����ķ���GL_SPOT_EXPONENT����ֻ��һ��ֵ����ʾ�۹�ĳ̶ȣ�Ϊ��ʱ��ʾ���շ�Χ�����������
�Ĺ���ǿ����ͬ��Ϊ����ʱ��ʾ���������뼯�У����Է��䷽���λ���ܵ�������գ�����λ���ܵ����ٹ��ա���ֵԽ�󣬾۹�Ч����Խ���ԡ�
GL_SPOT_CUTOFF����Ҳֻ��һ��ֵ����ʾһ���Ƕȣ����ǹ�Դ������������ǽǶȵ�һ�루��ͼ2������ȡֵ��Χ��0��90֮�䣬Ҳ����ȡ180
�������ֵ��ȡֵΪ180ʱ��ʾ��Դ������߸���360�ȣ�����ʹ�þ۹�ƣ���ȫ��Χ���䡣
��4��GL_CONSTANT_ATTENUATION��GL_LINEAR_ATTENUATION��GL_QUADRATIC_ATTENUATION���ԡ����������Ա�ʾ�˹�Դ�������Ĺ��ߵ�ֱ�ߴ�
�����ԣ���Щ����ֻ��λ���Թ�Դ��Ч������ʵ�����У����ߵ�ǿ�����ž�������Ӷ�������OpenGL��������������Ƴ���ɺ�����
˥������ = 1 / (k1 + k2 * d + k3 * k3 * d)
����d��ʾ���룬���ߵĳ�ʼǿ�ȳ���˥�����ӣ��͵õ���Ӧ����Ĺ���ǿ�ȡ�k1, k2, k3�ֱ����GL_CONSTANT_ATTENUATION, 
GL_LINEAR_ATTENUATION, GL_QUADRATIC_ATTENUATION��ͨ�������������������Ϳ��Կ��ƹ����ڴ��������еļ������ơ�
���Ի����ǲ��١���Ȼ�ˣ������ʹ�÷����Թ�Դ����3����4�����������ԾͲ����õ��ˣ�����ͱ�ü�����
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