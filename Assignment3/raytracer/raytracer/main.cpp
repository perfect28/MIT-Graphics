#include <cstdio>
#include <cstring>
#include <assert.h>
#include <stdlib.h>
#include <limits>
#include "scene_parser.h"
#include "image.h"
#include "camera.h"
#include "group.h"
#include "hit.h"
#include "ray.h"
#include "light.h"
#include "glCanvas.h"

char *input_file = NULL;
int width = 100;
int height = 100;
char *output_file = NULL;
float depth_min = 0;
float depth_max = 1;
char *depth_file = NULL;
char *normal_file = NULL;
SceneParser* sceneParser = NULL;
extern int phi;
extern int theta;
extern bool gouraud_flag;

/*
raytracer框架：

外层循环每一个扫描点，发射一条视线光
所需参数：ray,hit,

for each pixel
	Cast a ray and find the intersection point
	IF there is an intersection
		color = ambient
		FOR each light
			color += shading from the light(depending on light property and material property)
		return color
	ELSE
		return background color
*/

Vec3f find_color(Ray ray,Hit hit,Group* group,Camera* camera)
{
	int num_lights = sceneParser->getNumLights();
	Vec3f cambient = sceneParser->getAmbientLight();
	if (group->intersect(ray, hit, camera->getTMin()))//撞到了
	{
		Vec3f cobject = hit.getMaterial()->getDiffuseColor();
		Vec3f canswer = cambient * cobject;
		Vec3f clight;
		Vec3f light_dir;
		Vec3f normal_dir = hit.getNormal();
		float distolight;
		for (int i = 0; i < num_lights; i++)
		{
			Light *light = sceneParser->getLight(i);
			//light_dir : the direction to the light
			// 该方法用于获得指向光的方向，光的颜色，和到达光的距离
			light->getIllumination(hit.getIntersectionPoint(), light_dir, clight, distolight);
			//cpixel  =  cambient * cobject + SUMi [ clamped(Li . N) * clighti * cobject ]
			//返回局部光
			canswer = canswer + hit.getMaterial()->Shade(ray, hit, light_dir, clight)*cobject;
			canswer.Clamp();
		}
		return canswer;
	}
	else
		return sceneParser->getBackgroundColor();
}

void rayTracer()
{
	Image *image = new Image(width, height);

	image->SetAllPixels(sceneParser->getBackgroundColor());

	Camera *camera = sceneParser->getCamera();
	Group *group = sceneParser->getGroup();

	for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++)
	{
		float x = i*1.0 / width;
		float y = j*1.0 / height;
		Ray ray = camera->generateRay(Vec2f(x, y));
		Vec3f init_normal(0, 0, 0);
		Hit hit(0.0f, NULL, init_normal);
		//ray:带入参数  hit:带出信息
		Vec3f canswer = find_color(ray,hit,group,camera);
		image->SetPixel(i, j, canswer);
	}

	image->SaveTGA(output_file);
}
int main(int argc, char* argv[])
{
	// sample command line:
	// raytracer -input scene1_1.txt -size 200 200 -output output1_1.tga -depth 9 10 depth1_1.tga
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-input")) {
			i++; assert(i < argc);
			input_file = argv[i];
		}
		else if (!strcmp(argv[i], "-size")) {
			i++; assert(i < argc);
			width = atoi(argv[i]);
			i++; assert(i < argc);
			height = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-output")) {
			i++; assert(i < argc);
			output_file = argv[i];
		}
		else if (!strcmp(argv[i], "-depth")) {
			i++; assert(i < argc);
			depth_min = atof(argv[i]);
			i++; assert(i < argc);
			depth_max = atof(argv[i]);
			i++; assert(i < argc);
			depth_file = argv[i];
		}
		else if (!strcmp(argv[i], "-normals")){
			i++; assert(i < argc);
			normal_file = argv[i];
		}
		else if (!strcmp(argv[i], "-gui")){
		}
		else if (!strcmp(argv[i], "-tessellation")){
			i++; assert(i < argc);
			theta = atoi(argv[i]);
			i++; assert(i < argc);
			phi = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-gouraud")){
			gouraud_flag = true;
		}
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}
	//Use the input file parsing code provided to load the camera, 
	//background color and objects of the scene.
	sceneParser = new SceneParser(input_file);

	GLCanvas* glCanvas = new GLCanvas();

	glCanvas->initialize(sceneParser, rayTracer);
	return 0;
}