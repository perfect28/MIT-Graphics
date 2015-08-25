#include "rayTracer.h"
#include "group.h"
#include "camera.h"
#include "light.h"
#include "rayTree.h"

bool shade_back = false;

RayTracer::RayTracer(SceneParser *s, int max_bounces, float cutoff_weight, bool shadows, ...)
{
	this->sceneParser = s;
	this->max_bounces = max_bounces;
	this->cufoff_weight = cutoff_weight;
	this->shadow = shadows;
}

RayTracer::~RayTracer()
{
}

//Vector3 CalculateR(Vector3 &N, Vector3 &to) {
//	N.Normalize();
//	to.Normalize();
//	Vector3 returns = to - N * (to * N) * 2;
//	return returns;
//}
//
//Vector3 CalculateT(Vector3 &N, Vector3 &to, float param) {
//	Vector3 face;
//	float theta1;
//	float theta2;
//	Vector3 returns;
//	if (to * N < 0) {
//		face = (to - N * (to * N)).Normalize();
//		theta1 = acos(-N * to);
//		if (sin(theta1) / param > 1) {
//			return Params::UnableToSee;
//		}
//		theta2 = asin(sin(theta1) / param);
//		returns = -N * cos(theta2) + face * sin(theta2);
//		return returns;
//	}
//	else {
//		face = (to + N * (to * N)).Normalize();
//		theta1 = acos(N * to);
//		if (sin(theta1) * param > 1) {
//			return Params::UnableToSee;
//		}
//		theta2 = asin(sin(theta1) * param);
//		returns = N * cos(theta2) + face * sin(theta2);
//		return returns;
//	}
//}



Vec3f mirrorDirection(const Vec3f &normal, const Vec3f &incoming)
{
	//(反射光方向：R = V - 2(V.N)N )
	Vec3f reflectDir = incoming - 2 * (incoming.Dot3(normal))*normal;
	reflectDir.Normalize();
	return reflectDir;
}

bool transmittedDirection(const Vec3f &normal, const Vec3f &incoming,
	float index_i, float index_t, Vec3f &transmitted)
{
	//折射光方向：T = e(N.I) - sqrt(1 - e ^ 2(1 - (N.I) ^ 2)))N - eI
	//从物体内部向外：T = (-N.I - sqrt(1 - 1 / e ^ 2(1 - N.I) ^ 2) * 1 / e)N - I / e
	//The dot product of the normal and ray direction is negative when we are outside 
	//the object, and positive when we are inside. 

	//不需要在这里判断光线是在内还是在外，求法是一样的
	//不同的是一方面normal是反的，另一方面比值是互倒的
	Vec3f I = incoming;
	I.Negate();
	float e = index_i / index_t;
	float tmp = normal.Dot3(I);
	float sqrt_value = 1 - e*e*(1 - tmp*tmp);
	if (sqrt_value < 0)
		return false;
	transmitted = (e*tmp - sqrt(sqrt_value))*normal - e*I;
	return true;
}

/*
bounces:当前递归深度
weight:这条光线的贡献百分比

trace ray(ray)
	Intersect all objects
	IF there is an intersection
		color = ambient term
		For every light
			cast shadow ray(hit 获取焦点，方向指向光源)
			color += local shading term
		If mirror(反射光方向：R = V - 2(V.N)N )
			color += color(refl)*trace reflected ray
		If transparent
			(折射光方向：T = e(N.I)-sqrt(1-e^2(1-(N.I)^2)))N - eI)
			(从物体内部向外：T = (-N.I - sqrt(1-1/e^2(1-N.I)^2)*1/e)N-I/e)
			color += color(trans)*trace transmitted ray
*/

bool is_view_ray = true;

Vec3f RayTracer::traceRay(Ray &ray, float tmin, int bounces, float weight,
	float indexOfRefraction, Hit &hit) const
{
	//printf("当前已有光线：\n");
	//RayTree::Print();

	Vec3f canswer;
	if (bounces > max_bounces)
		return Vec3f(0.0f, 0.0f, 0.0f); 
	Camera *camera = sceneParser->getCamera();
	Group *group = sceneParser->getGroup();
	int num_lights = sceneParser->getNumLights();
	Vec3f cambient = sceneParser->getAmbientLight();
	//原来最后是这里出了问题，一旦碰到有转换的物体，那么hit带出来的值是
	//转换后的视线看到的值，而非本来视线看到的值
	//所以解决方案是：距离不变，根据距离重新计算焦点
	if (group->intersect(ray, hit, tmin))//撞到了
	{
		if (is_view_ray)
		{
			RayTree::SetMainSegment(ray, 0, hit.getT());
			is_view_ray = false;
		}
		Vec3f cobject = hit.getMaterial()->getDiffuseColor();
		Vec3f hitPoint = hit.getIntersectionPoint();
		//环境光部分
		canswer = cambient * cobject;
		Vec3f clight;//光的颜色
		Vec3f light_dir;//指向光的方向
		Vec3f normal_dir = hit.getNormal();//交点法线向量
		float distolight;//距离光源的距离
		for (int i = 0; i < num_lights; i++)
		{
			Light *light = sceneParser->getLight(i);
			//light_dir : the direction to the light
			// 该方法用于获得指向光的方向，光的颜色，和到达光的距离
			// 第一个参数传递的是焦点信息
			light->getIllumination(hitPoint, light_dir, clight, distolight);

			Ray ray2(hitPoint, light_dir);
			Vec3f init_normal(0, 0, 0);
			Hit hit2(distolight, NULL, init_normal);
			//阴影检测
			if (shadow)
			{
				if (group->intersect(ray2, hit2, tmin)){
					RayTree::AddShadowSegment(ray2, 0, hit2.getT());
					continue;
				}
				RayTree::AddShadowSegment(ray2, 0, hit2.getT());
			}
			//cpixel  =  cambient * cobject + SUMi [ clamped(Li . N) * clighti * cobject ]
			//返回局部光
			canswer = canswer + hit.getMaterial()->Shade(ray, hit, light_dir, clight);
		}

		//printf("当前已有光线：\n");
		//RayTree::Print();

		
		//反射光
		Material *material = hit.getMaterial();
		Vec3f rc = material->getReflectiveColor();
		if (rc.r() > 0 && rc.g() > 0 && rc.b() > 0)
		{
			Vec3f mirrorDir;
			Vec3f incoming = ray.getDirection();
			mirrorDir = mirrorDirection(normal_dir, incoming);
			// The ray weight is simply multiplied by the magnitude of the reflected color
			Ray ray3(hitPoint, mirrorDir);
			Vec3f init_normal(0, 0, 0);
			Hit hit3(distolight, NULL, init_normal);
			//忘记乘以本身的反射光系数%…………
			canswer += traceRay(ray3, tmin, bounces + 1, weight*rc.Length(), indexOfRefraction, hit3)*rc;
			if (bounces + 1 < max_bounces)
				RayTree::AddReflectedSegment(ray3, 0, hit3.getT());
		}

		//printf("当前已有光线：\n");
		//RayTree::Print();


		//从这里开始还都存在问题！！！！！
		//折射光
		Vec3f transmitted;
		Vec3f tc = material->getTransparentColor();
		float index = material->getIndexOfRefraction();
		if (tc.r() > 0 && tc.g() > 0 && tc.b() > 0)
		{
			Vec3f init_normal(0, 0, 0);
			Hit hit4(distolight, NULL, init_normal);
			//在判断折射光的存在之后，要考虑光线的位置：物体内还是物体外
			//这里根据normal和incoming的点积来判断
			Vec3f incoming = ray.getDirection();
			float judge = normal_dir.Dot3(incoming);
			if (judge < 0)//光线在外
			{
				if (transmittedDirection(normal_dir, incoming, 1, index, transmitted))
				{
					Ray ray4(hitPoint, transmitted);
					canswer += traceRay(ray4, tmin, bounces+1, weight*rc.Length(), index, hit4)*tc;
					RayTree::AddTransmittedSegment(ray4, 0, hit4.getT());
				}
			}
			else//光线在内
			{
				normal_dir.Negate();
				if (transmittedDirection(normal_dir, incoming, index, 1, transmitted))
				{
					Ray ray4(hitPoint, transmitted);
					canswer += traceRay(ray4, tmin, bounces+1, weight*rc.Length(), 1, hit4)*tc;
					RayTree::AddTransmittedSegment(ray4, 0, hit4.getT());
				}
			}
		}

		//printf("当前已有光线：\n");
		//RayTree::Print();

	}
	else
		canswer = sceneParser->getBackgroundColor();

	canswer.Clamp();
	return canswer;
}