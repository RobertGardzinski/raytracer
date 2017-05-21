// RaytracerProject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	int height = 800;
	int width = 1000;
	int bpp = 24;

	cout << "Resolution: " << width << "x" << height << endl;

	CImage image = CImage(width, height, bpp);
	image.init();

	CCamera camera;

	CScene scene;
	scene.cam = camera;

	CRayTrace rayTrace;

	float r1 = 2.0;
	float r2 = 1.0;
	glm::vec3 center1 = glm::vec3(0.0, 0.0, 0.0);
	//glm::vec3 center2 = glm::vec3(-2.0, 2.0, -2.0);
	glm::vec3 center2 = glm::vec3(2.0, 2.0, 0.0);
	int reflect1 = 0;
	int reflect2 = reflect1;
	int reflectt = 1;
	glm::vec3 ambient1 = glm::vec3(0.01, 0.01, 0.01);
	//glm::vec3 ambient1 = glm::vec3(0.01, 0.1, 0.5);
	glm::vec3 ambient2 = glm::vec3(0.01, 0.01, 0.01);
	glm::vec3 ambientt = glm::vec3(0.01, 0.01, 0.01);
	glm::vec3 diffuse1 = glm::vec3(0.8, 0.01, 0.01);
	glm::vec3 diffuse2 = glm::vec3(0.01, 0.8, 0.01);
	glm::vec3 diffuset = glm::vec3(0.01, 0.01, 0.6);
	glm::vec3 specular1 = glm::vec3(0.8, 0.8, 0.8);
	glm::vec3 specular2 = glm::vec3(0.01, 0.01, 0.01);
	glm::vec3 speculart = glm::vec3(0.01, 0.01, 0.01);
	int shininess1 = 20;
	int shininess2 = shininess1;
	int shininesst = shininess1;
	Bitmap earth = Bitmap("Resources/Textures/earth.bmp");
	Bitmap mars = Bitmap("Resources/Textures/mars.bmp");
	//texture.data - { r, g, b, r, g, b, ... }
	CSphere sphere1 = CSphere(r1, center1, reflect1, ambient1, diffuse1, specular1, shininess1, earth.data, earth.width, earth.height);
	CSphere sphere2 = CSphere(r2, center2, reflect2, ambient2, diffuse2, specular2, shininess2, mars.data, mars.width, mars.height);

	glm::vec3 p11 = glm::vec3(-5.0, -4.0, -5.0);
	glm::vec3 p12 = glm::vec3(-5.0, -4.0, 5.0);
	glm::vec3 p13 = glm::vec3(5.0, -4.0, 5.0);
	glm::vec3 p21 = glm::vec3(5.0, -4.0, 5.0);
	glm::vec3 p22 = glm::vec3(5.0, -4.0, -5.0);
	glm::vec3 p23 = glm::vec3(-5.0, -4.0, -5.0);
	Bitmap steel = Bitmap("Resources/Textures/wood.bmp");
	Bitmap steel2 = Bitmap("Resources/Textures/wood2.bmp");
	CTriangle triangle1 = CTriangle(p12, p13, p11, reflectt, ambientt, diffuset, speculart, shininesst, steel.data, steel.width, steel.height);
	CTriangle triangle2 = CTriangle(p22, p23, p21, reflectt, ambientt, diffuset, speculart, shininesst, steel2.data, steel.width, steel.height);
	
	glm::vec3 pl11 = glm::vec3(-4.0, 4.0, -2.0);
	glm::vec3 pl21 = glm::vec3(1.0, 1.0, -10.0);
	glm::vec3 ambient1l = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 ambient2l = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 diffuse1l = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 diffuse2l = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 specular1l = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 specular2l = glm::vec3(1.0, 1.0, 1.0);
	CLight light1 = CLight(pl11, ambient1l, diffuse1l, specular1l);
	CLight light2 = CLight(pl21, ambient1l, diffuse1l, specular1l);


	scene.addObject(&sphere1);
	scene.addObject(&sphere2);
	scene.addObject(&triangle1);
	scene.addObject(&triangle2);

	scene.addLight(&light2);
	scene.addLight(&light1);
	
	rayTrace.run(&scene, image, false);

	// Prowizoryczna głębia ostrości
	/*float offset = 0.08;
	
	cout << "Image Left: 1" << endl;
	CImage imageL1 = CImage(width, height, bpp);
	imageL1.init();
	camera.eyep = glm::vec3(3.0f + offset, 15.0f, -16.0f);
	scene.cam = camera;
	rayTrace.run(&scene, imageL1, false);
	cout << "Image Left: 2" << endl;
	CImage imageL2 = CImage(width, height, bpp);
	imageL2.init();
	camera.eyep = glm::vec3(3.0f + offset * 2, 15.0f, -16.0f);
	scene.cam = camera;
	rayTrace.run(&scene, imageL2, false);
	cout << "Image Left: 3" << endl;
	CImage imageL3 = CImage(width, height, bpp);
	imageL3.init();
	camera.eyep = glm::vec3(3.0f + offset * 3, 15.0f, -16.0f);
	scene.cam = camera;
	rayTrace.run(&scene, imageL3, false);
	cout << "Image Left: 4" << endl;
	CImage imageL4 = CImage(width, height, bpp);
	imageL4.init();
	camera.eyep = glm::vec3(3.0f + offset * 4, 15.0f, -16.0f);
	scene.cam = camera;
	rayTrace.run(&scene, imageL4, false);

	cout << "Image Right: 1" << endl;
	CImage imageR1 = CImage(width, height, bpp);
	imageR1.init();
	camera.eyep = glm::vec3(3.0f - offset, 15.0f, -16.0f);
	scene.cam = camera;
	rayTrace.run(&scene, imageR1, false);
	cout << "Image Right: 2" << endl;
	CImage imageR2 = CImage(width, height, bpp);
	imageR2.init();
	camera.eyep = glm::vec3(3.0f - offset * 2, 15.0f, -16.0f);
	scene.cam = camera;
	rayTrace.run(&scene, imageR2, false);
	cout << "Image Right: 3" << endl;
	CImage imageR3 = CImage(width, height, bpp);
	imageR3.init();
	camera.eyep = glm::vec3(3.0f - offset * 3, 15.0f, -16.0f);
	scene.cam = camera;
	rayTrace.run(&scene, imageR3, false);
	cout << "Image Right: 4" << endl;
	CImage imageR4 = CImage(width, height, bpp);
	imageR4.init();
	camera.eyep = glm::vec3(3.0f - offset * 4, 15.0f, -16.0f);
	scene.cam = camera;
	rayTrace.run(&scene, imageR4, false);
	cout << "Finishing..." << endl;

	glm::vec3 vecColor = glm::vec3(0, 0, 0);
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			vecColor = image.getPixelColor(i, j)
				+ imageL1.getPixelColor(i, j)
				+ imageL2.getPixelColor(i, j)
				+ imageL3.getPixelColor(i, j)
				+ imageL4.getPixelColor(i, j)
				+ imageR1.getPixelColor(i, j)
				+ imageR2.getPixelColor(i, j)
				+ imageR3.getPixelColor(i, j)
				+ imageR4.getPixelColor(i, j);		
				
			image.setPixelColor(i, j, vecColor.x/9, vecColor.y/9, vecColor.z/9);
		}
	}*/

	image.save();

	return 0;
}

