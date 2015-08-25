// ====================================================================
// OPTIONAL: 3 pass rendering to fix the specular highlight 
// artifact for small specular exponents (wide specular lobe)
// ====================================================================

// include glCanvas.h to access the preprocessor variable SPECULAR_FIX
#include "glCanvas.h"  
#include "material.h"
#include <GL/glut.h>

#ifdef SPECULAR_FIX
// OPTIONAL:  global variable allows (hacky) communication 
// with glCanvas::display
extern int SPECULAR_FIX_WHICH_PASS;
#endif


Material::Material()
{

}

Material::~Material()
{

}

PhongMaterial::PhongMaterial(const Vec3f &diffuseColor,
	const Vec3f &specularColor,
	float exponent,
	const Vec3f &reflectiveColor,
	const Vec3f &transparentColor,
	float indexOfRefraction)
{
	this->diffuseColor = diffuseColor;
	this->specularColor = specularColor;
	this->exponent = exponent;
	this->reflectiveColor = reflectiveColor;
	this->transparentColor = transparentColor;
	this->indexOfRefraction = indexOfRefraction;
}

PhongMaterial::~PhongMaterial()
{

}

//该方法就是返回局部光照的值
Vec3f PhongMaterial::Shade(const Ray &ray, const Hit &hit, const Vec3f &dirToLight,
	const Vec3f &lightColor) const
{
	Vec3f canswer;
	Vec3f clight = lightColor;
	Vec3f light_dir = dirToLight;
	Vec3f normal_dir = hit.getNormal();
	//light_dir : the direction to the light
	// 该方法用于获得指向光的方向，光的颜色，和到达光的距离
	//cpixel  =  cambient * cobject + SUMi [ clamped(Li . N) * clighti * cobject ]
	//用float变量保存点积值……
	float tmp = light_dir.Dot3(normal_dir);
	if (tmp < 0) tmp = 0;
	canswer = tmp * clight*diffuseColor;

	//高光部分
	Vec3f ray_dir = ray.getDirection();
	ray_dir.Negate();
	float de = (ray_dir + light_dir).Length();
	Vec3f h = (ray_dir + light_dir);
	h.Divide(de, de, de);
	tmp = 1.0f;
	for (int i = 1; i <= exponent; i++)
		tmp *= normal_dir.Dot3(h);
	canswer += tmp*clight*specularColor;
	return canswer;
}

Vec3f PhongMaterial::getSpecularColor() const
{
	return specularColor;
}

Vec3f PhongMaterial::getReflectiveColor() const
{
	return reflectiveColor;
}
Vec3f PhongMaterial::getTransparentColor() const
{
	return transparentColor;
}

float PhongMaterial::getIndexOfRefraction() const
{
	return indexOfRefraction;
}

// ====================================================================
// Set the OpenGL parameters to render with the given material
// attributes.
// ====================================================================

void PhongMaterial::glSetMaterial(void) const {

	GLfloat one[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat zero[4] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat specular[4] = {
		getSpecularColor().r(),
		getSpecularColor().g(),
		getSpecularColor().b(),
		1.0 };
	GLfloat diffuse[4] = {
		getDiffuseColor().r(),
		getDiffuseColor().g(),
		getDiffuseColor().b(),
		1.0 };

	// NOTE: GL uses the Blinn Torrance version of Phong...      
	float glexponent = exponent;
	if (glexponent < 0) glexponent = 0;
	if (glexponent > 128) glexponent = 128;

#if !SPECULAR_FIX 

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

#else

	// OPTIONAL: 3 pass rendering to fix the specular highlight 
	// artifact for small specular exponents (wide specular lobe)

	if (SPECULAR_FIX_WHICH_PASS == 0) {
		// First pass, draw only the specular highlights
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

	}
	else if (SPECULAR_FIX_WHICH_PASS == 1) {
		// Second pass, compute normal dot light 
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, one);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
	}
	else {
		// Third pass, add ambient & diffuse terms
		assert(SPECULAR_FIX_WHICH_PASS == 2);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
	}

#endif
}

// ====================================================================
// ====================================================================