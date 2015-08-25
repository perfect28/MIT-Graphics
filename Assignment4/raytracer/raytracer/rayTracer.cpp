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
	//(����ⷽ��R = V - 2(V.N)N )
	Vec3f reflectDir = incoming - 2 * (incoming.Dot3(normal))*normal;
	reflectDir.Normalize();
	return reflectDir;
}

bool transmittedDirection(const Vec3f &normal, const Vec3f &incoming,
	float index_i, float index_t, Vec3f &transmitted)
{
	//����ⷽ��T = e(N.I) - sqrt(1 - e ^ 2(1 - (N.I) ^ 2)))N - eI
	//�������ڲ����⣺T = (-N.I - sqrt(1 - 1 / e ^ 2(1 - N.I) ^ 2) * 1 / e)N - I / e
	//The dot product of the normal and ray direction is negative when we are outside 
	//the object, and positive when we are inside. 

	//����Ҫ�������жϹ��������ڻ������⣬����һ����
	//��ͬ����һ����normal�Ƿ��ģ���һ�����ֵ�ǻ�����
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
bounces:��ǰ�ݹ����
weight:�������ߵĹ��װٷֱ�

trace ray(ray)
	Intersect all objects
	IF there is an intersection
		color = ambient term
		For every light
			cast shadow ray(hit ��ȡ���㣬����ָ���Դ)
			color += local shading term
		If mirror(����ⷽ��R = V - 2(V.N)N )
			color += color(refl)*trace reflected ray
		If transparent
			(����ⷽ��T = e(N.I)-sqrt(1-e^2(1-(N.I)^2)))N - eI)
			(�������ڲ����⣺T = (-N.I - sqrt(1-1/e^2(1-N.I)^2)*1/e)N-I/e)
			color += color(trans)*trace transmitted ray
*/

bool is_view_ray = true;

Vec3f RayTracer::traceRay(Ray &ray, float tmin, int bounces, float weight,
	float indexOfRefraction, Hit &hit) const
{
	//printf("��ǰ���й��ߣ�\n");
	//RayTree::Print();

	Vec3f canswer;
	if (bounces > max_bounces)
		return Vec3f(0.0f, 0.0f, 0.0f); 
	Camera *camera = sceneParser->getCamera();
	Group *group = sceneParser->getGroup();
	int num_lights = sceneParser->getNumLights();
	Vec3f cambient = sceneParser->getAmbientLight();
	//ԭ�����������������⣬һ��������ת�������壬��ôhit��������ֵ��
	//ת��������߿�����ֵ�����Ǳ������߿�����ֵ
	//���Խ�������ǣ����벻�䣬���ݾ������¼��㽹��
	if (group->intersect(ray, hit, tmin))//ײ����
	{
		if (is_view_ray)
		{
			RayTree::SetMainSegment(ray, 0, hit.getT());
			is_view_ray = false;
		}
		Vec3f cobject = hit.getMaterial()->getDiffuseColor();
		Vec3f hitPoint = hit.getIntersectionPoint();
		//�����ⲿ��
		canswer = cambient * cobject;
		Vec3f clight;//�����ɫ
		Vec3f light_dir;//ָ���ķ���
		Vec3f normal_dir = hit.getNormal();//���㷨������
		float distolight;//�����Դ�ľ���
		for (int i = 0; i < num_lights; i++)
		{
			Light *light = sceneParser->getLight(i);
			//light_dir : the direction to the light
			// �÷������ڻ��ָ���ķ��򣬹����ɫ���͵����ľ���
			// ��һ���������ݵ��ǽ�����Ϣ
			light->getIllumination(hitPoint, light_dir, clight, distolight);

			Ray ray2(hitPoint, light_dir);
			Vec3f init_normal(0, 0, 0);
			Hit hit2(distolight, NULL, init_normal);
			//��Ӱ���
			if (shadow)
			{
				if (group->intersect(ray2, hit2, tmin)){
					RayTree::AddShadowSegment(ray2, 0, hit2.getT());
					continue;
				}
				RayTree::AddShadowSegment(ray2, 0, hit2.getT());
			}
			//cpixel  =  cambient * cobject + SUMi [ clamped(Li . N) * clighti * cobject ]
			//���ؾֲ���
			canswer = canswer + hit.getMaterial()->Shade(ray, hit, light_dir, clight);
		}

		//printf("��ǰ���й��ߣ�\n");
		//RayTree::Print();

		
		//�����
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
			//���ǳ��Ա���ķ����ϵ��%��������
			canswer += traceRay(ray3, tmin, bounces + 1, weight*rc.Length(), indexOfRefraction, hit3)*rc;
			if (bounces + 1 < max_bounces)
				RayTree::AddReflectedSegment(ray3, 0, hit3.getT());
		}

		//printf("��ǰ���й��ߣ�\n");
		//RayTree::Print();


		//�����￪ʼ�����������⣡��������
		//�����
		Vec3f transmitted;
		Vec3f tc = material->getTransparentColor();
		float index = material->getIndexOfRefraction();
		if (tc.r() > 0 && tc.g() > 0 && tc.b() > 0)
		{
			Vec3f init_normal(0, 0, 0);
			Hit hit4(distolight, NULL, init_normal);
			//���ж������Ĵ���֮��Ҫ���ǹ��ߵ�λ�ã������ڻ���������
			//�������normal��incoming�ĵ�����ж�
			Vec3f incoming = ray.getDirection();
			float judge = normal_dir.Dot3(incoming);
			if (judge < 0)//��������
			{
				if (transmittedDirection(normal_dir, incoming, 1, index, transmitted))
				{
					Ray ray4(hitPoint, transmitted);
					canswer += traceRay(ray4, tmin, bounces+1, weight*rc.Length(), index, hit4)*tc;
					RayTree::AddTransmittedSegment(ray4, 0, hit4.getT());
				}
			}
			else//��������
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

		//printf("��ǰ���й��ߣ�\n");
		//RayTree::Print();

	}
	else
		canswer = sceneParser->getBackgroundColor();

	canswer.Clamp();
	return canswer;
}