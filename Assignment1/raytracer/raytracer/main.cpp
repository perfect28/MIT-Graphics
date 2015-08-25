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

int main(int argc, char* argv[])
{

	// ========================================================
	// ========================================================
	// Some sample code you might like to use for parsing 
	// command line arguments 

	char *input_file = NULL;
	int width = 100;
	int height = 100;
	char *output_file = NULL;
	float depth_min = 0;
	float depth_max = 1;
	char *depth_file = NULL;

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
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}

	// ========================================================
	// ========================================================

	//Use the input file parsing code provided to load the camera, 
	//background color and objects of the scene.
	SceneParser* sceneParser = new SceneParser(input_file);

	Image *image = new Image(width, height);
	Image *image2 = new Image(width, height);
	image->SetAllPixels(sceneParser->getBackgroundColor());
	image2->SetAllPixels(sceneParser->getBackgroundColor());
	Camera *camera = sceneParser->getCamera();
	Group *group = sceneParser->getGroup();

	Vec3f black(0.0f, 0.0f, 0.0f);
	Material *init = new Material(black);

	/*
	Write a main function that reads the scene (using the parsing code provided),
	loops over the pixels in the image plane,
	generates a ray using your OrthographicCamera class,
	intersects it with the high-level Group that stores the objects of the scene,
	and writes the color of the closest intersected object.
	*/
	/*
	ray casting:
	for every pixel
	construct a ray from the eye
	for every object in the scene
	find intersection with the ray
	keep if closest
	*/

	for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++)
	{
		float x = i*1.0 / width;
		float y = j*1.0 / height;
		Ray ray = camera->generateRay(Vec2f(x, y));

		Hit hit(0.0f, init);
		//ray:带入参数  hit:带出信息
		if (group->intersect(ray, hit, camera->getTMin()))//撞到了
		{
			image->SetPixel(i,j,hit.getMaterial()->getDiffuseColor());

			//k值越大说明越远，颜色应该越深
			float k = (hit.getT() - depth_min)*1.0 / (depth_max - depth_min);
			if (k > 1.0f) k = 1.0f;
			if (k < 0.0f) k = 0.0f;
			Vec3f gray(1.0f - k, 1.0f - k, 1.0f - k);
			image2->SetPixel(i, j, gray);
		}
	}

	image->SaveTGA(output_file);
	image2->SaveTGA(depth_file);

	/*
	Implement a second rendering style to visualize the depth t of objects in the scene. 
	Two input depth values specify the range of depth values which should be mapped 
	to shades of gray in the visualization. Depth values outside this range are simply clamped.
	*/

	return 0;
}