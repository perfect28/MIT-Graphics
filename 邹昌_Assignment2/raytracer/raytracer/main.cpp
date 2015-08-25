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
	char *normal_file = NULL;
	bool shade_back_flag = false;

	// sample command line:
	// raytracer -input scene1_1.txt -size 200 200 -output output1_1.tga -depth 9 10 depth1_1.tga

	/*
	Implement the new rendering mode, normal visualization. Add code to parse an 
	additional command line option -normals <normal_file.tga> to specify the output 
	file for this visualization (see examples below).
	*/

	/*
	In test scenes 5 & 7 below, we've asked you to render the "wrong" or "back" side of both a Sphere 
	and a Triangle primitive. Add the -shade_back option to your raytracer. When this option is specified, 
	we'd like you to treat both sides of your object surfaces in the same manner. This means you'll need
	to flip the normal when the eye is on the "wrong" side of the surface (when the dot product of the 
	ray direction & the normal is positive). Do this normal flip just before you shade a pixel, not within
	the object intersection code. If the -shade_back flag is not specified, you should shade back-facing 
	surfaces differently, to aid in debugging. Back-facing surfaces must be detected to implement refraction 
	through translucent objects, and are often not rendered at all for efficiency in real-time applications.
	We'll see this again in upcoming lectures and assignments.
	*/
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
		else if (!strcmp(argv[i], "-shade_back")){
			shade_back_flag = true;
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
	Vec3f black(0.0f, 0.0f, 0.0f);
	Material *init_material = new Material(black);
	Image *image = new Image(width, height);
	Image *image_depth = new Image(width, height);
	Image *image_normal = new Image(width, height);
	image->SetAllPixels(sceneParser->getBackgroundColor());
	image_depth->SetAllPixels(sceneParser->getBackgroundColor());
	image_normal->SetAllPixels(black);

	Camera *camera = sceneParser->getCamera();
	Group *group = sceneParser->getGroup();
	
	
	/*
		Task:
		1.add new primitives (planes and triangles) and affine transformations
		2.implement a perspective camera
		3.implement two simple shading modes: normal visualization and diffuse shading.
	*/
	/*
	normal visualization:
	For the normal visualization, you will simply display the absolute value of 
	the coordinates of the normal vector as an (r, g, b) color. For example, a 
	normal pointing in the positive or negative z direction will be displayed as 
	pure blue (0, 0, 1). You should use black as the color for the background (undefined normal).

	Diffuse shading:
	Diffuse shading is our first step toward modeling the interaction of light and materials.
	Given the direction to the light L and the normal N we can compute the diffuse shading as a clamped dot product:

	d	 = L . N     	if L . N > 0
	= 0	otherwise

	If the visible object has color cobject = (r, g, b), and the light source has color clight = (Lr, Lg, Lb), 
	then the pixel color is cpixel = (rLrd, gLgd, bLbd). Multiple light sources are handled by simply summing their 
	contributions. We can also include an ambient light with color cambient, which can be very helpful in debugging. 
	Without it, parts facing away from the light source appear completely black. Putting this all together, 
	
	the formula is:

	cpixel  =  cambient * cobject + SUMi [ clamped(Li . N) * clighti * cobject ]
	*/

	int num_lights = sceneParser->getNumLights();
	Vec3f cambient = sceneParser->getAmbientLight();
	for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++)
	{
		float x = i*1.0 / width;
		float y = j*1.0 / height;
		Ray ray = camera->generateRay(Vec2f(x, y));

		Vec3f init_normal(0,0,0);
		Hit hit(0.0f, init_material,init_normal);
		//ray:带入参数  hit:带出信息
		if (group->intersect(ray, hit, camera->getTMin()))//撞到了
		{
			Vec3f cobject = hit.getMaterial()->getDiffuseColor();
			Vec3f canswer = cambient * cobject;
			Vec3f clight;
			Vec3f light_dir;
			Vec3f normal_dir;
			normal_dir = hit.getNormal();
			for (int i = 0; i < num_lights; i++)
			{
				Light *light = sceneParser->getLight(i);
				
				//light_dir : the direction to the light
				light->getIllumination(hit.getIntersectionPoint(), light_dir, clight);
				//cpixel  =  cambient * cobject + SUMi [ clamped(Li . N) * clighti * cobject ]
				//用float变量保存点积值……
				float tmp = light_dir.Dot3(normal_dir);
				if (shade_back_flag)
					if (tmp < 0) tmp = -tmp;
				canswer = canswer + tmp * clight * cobject;
				canswer.Clamp();
			}
			image->SetPixel(i,j,canswer);


			if (depth_file != NULL)
			{
				//k值越大说明越远，颜色应该越深
				float k = (hit.getT() - depth_min)*1.0 / (depth_max - depth_min);
				if (k > 1.0f) k = 1.0f;
				if (k < 0.0f) k = 0.0f;
				Vec3f gray(1.0f - k, 1.0f - k, 1.0f - k);
				image_depth->SetPixel(i, j, gray);
			}
			

			//For the normal visualization, you will simply display the absolute value of the
			//coordinates of the normal vector as an (r, g, b) color.
			if (normal_file != NULL)
			{
				normal_dir.Abs();
				image_normal->SetPixel(i, j, normal_dir);
			}
			
		}
	}

	image->SaveTGA(output_file);
	if (depth_file != NULL)
		image_depth->SaveTGA(depth_file);
	if (normal_file!=NULL)
		image_normal->SaveTGA(normal_file);

	/*
	Implement a second rendering style to visualize the depth t of objects in the scene. 
	Two input depth values specify the range of depth values which should be mapped 
	to shades of gray in the visualization. Depth values outside this range are simply clamped.
	*/

	return 0;
}