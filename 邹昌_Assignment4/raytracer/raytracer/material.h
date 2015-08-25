#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "vectors.h"
#include "hit.h"
#include "ray.h"
// ====================================================================
// ====================================================================

// You will extend this class in later assignments

class Material {

public:

	// CONSTRUCTORS & DESTRUCTOR
	Material();
	//Material(const Vec3f &d_color) { diffuseColor = d_color; }
	virtual ~Material();

	// ACCESSORS
	virtual Vec3f getDiffuseColor() const { return diffuseColor; }
	virtual Vec3f getSpecularColor() const = 0;
	virtual Vec3f getReflectiveColor() const = 0;
	virtual Vec3f getTransparentColor() const = 0;
	virtual float getIndexOfRefraction() const = 0;

	virtual Vec3f Shade
		(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
		const Vec3f &lightColor) const = 0;

	virtual void glSetMaterial(void) const = 0;
protected:

	// REPRESENTATION
	Vec3f diffuseColor;

};

// ====================================================================
// ====================================================================


class PhongMaterial :public Material
{
public:
	PhongMaterial::PhongMaterial(const Vec3f &diffuseColor,
		const Vec3f &specularColor,
		float exponent,
		const Vec3f &reflectiveColor,
		const Vec3f &transparentColor,
		float indexOfRefraction);
	~PhongMaterial();

	virtual Vec3f getSpecularColor() const;
	virtual Vec3f getReflectiveColor() const;
	virtual Vec3f getTransparentColor() const;
	virtual float getIndexOfRefraction() const;

	virtual Vec3f Shade
		(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
		const Vec3f &lightColor) const;

	virtual void glSetMaterial(void) const;
private:
	Vec3f specularColor;
	float exponent;
	Vec3f reflectiveColor;
	Vec3f transparentColor;
	float indexOfRefraction;
};

#endif