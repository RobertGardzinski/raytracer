#include "stdafx.h"
// Typy obiektów
#define OBJ_SPHERE 0
#define OBJ_TRIANGLE 1
#define OBJ_PLANE 2

using namespace std;

CObject::CObject() {
}

CSphere::CSphere(float r, glm::vec3 center, int reflect, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, int shininess, unsigned char* texture, int textureWidth, int textureHeight) {
	this->type = OBJ_SPHERE;
	this->r = r;
	this->center = center;
	this->reflect = reflect;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess;
	this->texture = texture;
	this->textureWidth = textureWidth;
	this->textureHeight = textureHeight;
}

glm::vec3 CSphere::texturing(unsigned char * texture, int textureWidth, int textureHeight, glm::vec3 point)
{
	glm::vec3 color;
	point = glm::normalize(point);
	const float pi = 3.14159265359;

	float s = 0.5 + (atan2(point.z, point.x) / (2 * pi));
	float t = 0.5 - asin(point.y) / pi;
	//printf("s = %4.2f | t = %4.2f\n", s, t);
	glm::vec2 P = glm::vec2((int)(s * textureWidth), (int)(t * textureHeight));
	
	const unsigned int index = P.y * textureWidth * 3 + P.x * 3;
	unsigned char Red = (unsigned char)(texture[index]);
	unsigned char Green = (unsigned char)(texture[index + 1]);
	unsigned char Blue = (unsigned char)(texture[index + 2]);
	color.x = (Red / 255.0f);
	color.y = (Green / 255.0f);
	color.z = (Blue / 255.0f);

	return color;
}

float CSphere::intersect(CRay* ray) {
	float result = -1.0f;
	float A = 1;
	float B = 2 * glm::dot(ray->dir, ray->pos - center);
	float C = pow(glm::distance(ray->pos, center), 2) - r*r;
	float d = B*B - 4 * A * C;
	if (d < 0) return result;
	float t1 = (-B + sqrt(d)) / (2 * A);
	float t2 = (-B - sqrt(d)) / (2 * A);
	if (t1>0)
	{
		if (t2>0)
		{
			if (t1 > t2)
			{
				result = t2;
			}
			else
			{
				result = t1;
			}
		}
		else
		{
			result = t1;
		}
	}
	else
	{
		result = t2;
	}

	glm::vec3 p = result * ray->dir + ray->pos;
	this->normal = glm::normalize(p - center);
	this->color = texturing(this->texture, this->textureWidth, this->textureHeight, p-center);
	this->diffuse = this->color;
	return result;
}

glm::vec3 CSphere::setNormal(CRay * ray)
{
	return ray->dir - center;
}

glm::vec3 CSphere::getAmbient()
{
	return this->ambient;
}

glm::vec3 CSphere::getSpecular()
{
	return this->specular;
}

glm::vec3 CSphere::getDiffuse()
{
	return this->diffuse;
}

glm::vec3 CSphere::getNormal()
{
	return glm::normalize(this->normal);
}

int CSphere::getShininess()
{
	return this->shininess;
}

int CScene::parse(char* fname) {
	return 0;
}

CImage::CImage(int aWidth, int aHeight, int aBPP) {
	height = aHeight;
	width = aWidth;
	BPP = aBPP;
}

void CImage::init() {
	start = (int)clock();
	FreeImage_Initialise();

	bitmap = FreeImage_Allocate(width, height, BPP);

	if (!bitmap)
	{
		cout << "Failed to allocate FreeImage";
		exit(1);
	}

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			color.rgbRed = 0;
			color.rgbGreen = 0;
			color.rgbBlue = 0;
			FreeImage_SetPixelColor(bitmap, i, j, &color);
		}
	}
}

void CImage::save() {
	int time = ((int)clock() - start) / CLOCKS_PER_SEC;
	string fname = "Images/" + std::to_string(time) + "s.png";
	const char * c = fname.c_str();
	if (FreeImage_Save(FIF_PNG, bitmap, c, 0))
		cout << "Image successfully saved!" << endl;

	FreeImage_DeInitialise();
}

void CImage::setPixelColor(int i, int j, int R, int G, int B) {
	color.rgbRed = R;
	color.rgbGreen = G;
	color.rgbBlue = B;
	FreeImage_SetPixelColor(bitmap, i, height - 1 - j, &color);	
}

glm::vec3 CImage::getPixelColor(int i, int j)
{
	FreeImage_GetPixelColor(bitmap, i, height - 1 - j, &color);	
	return glm::vec3(color.rgbRed, color.rgbGreen, color.rgbBlue);
}

int CRayTrace::rayTrace(CRay &ray, CScene* scene, COutput* res) {
	float t;
	CObject* object;
	object = scene->find_intersection(ray, true, &t);	
	
	if (object == nullptr) return 0;
	
	int mShi = object->getShininess();
	glm::vec3 p = t*ray.dir + ray.pos;
	glm::vec3 n = object->getNormal();
	glm::vec3 v = glm::normalize(scene->cam.eyep - p);
	glm::vec3 l;
	glm::vec3 r;
	float ln;
	float vr;

	glm::vec3 mAmb = object->getAmbient();
	glm::vec3 Amb = glm::vec3(0, 0, 0);
	glm::vec3 mDif = object->getDiffuse();
	glm::vec3 Dif = glm::vec3(0, 0, 0);
	glm::vec3 mSpe = object->getSpecular();
	glm::vec3 Spe = glm::vec3(0, 0, 0);


	for (int i = 0; i < scene->lightCount; i++) {
		l = glm::normalize(scene->lights[i]->getPosition() - p);
		r = glm::normalize(2 * (glm::dot(n, l)) * n - l);
		
		CRay light = CRay(scene->lights[i]->getPosition(), glm::normalize(p - scene->lights[i]->getPosition()));

		float lightDistance = glm::length(p - scene->lights[i]->getPosition());
		float lightClosestIntersect = scene->lightTrace(light, true);
		vr = pow(glm::dot(v, r), mShi);
		if (vr < 0 || 0.99*lightDistance > lightClosestIntersect)
			vr = 0;
		ln = glm::dot(l, n);
		if (ln < 0 || 0.99*lightDistance > lightClosestIntersect) 
			ln = 0;
		glm::vec3 sDif = scene->lights[i]->getDiffuse();
		Dif.x += (mDif.x * sDif.x * ln);
		Dif.y += (mDif.y * sDif.y * ln);
		Dif.z += (mDif.z * sDif.z * ln);
		glm::vec3 sSpe = scene->lights[i]->getSpecular();
		Spe.x += (mSpe.x * sSpe.x * vr);
		Spe.y += (mSpe.y * sSpe.y * vr);
		Spe.z += (mSpe.z * sSpe.z * vr);
		glm::vec3 sAmb = scene->lights[i]->getAmbient();
		Amb.x += mAmb.x * sAmb.x;
		Amb.y += mAmb.y * sAmb.y;
		Amb.z += mAmb.z * sAmb.z; 
	}

	float attenuation = 1.0;
	
	res->color[0] +=
		res->energy * (Amb.x + Dif.x +Spe.x);
	res->color[1] +=
		res->energy * (Amb.y + Dif.y +Spe.y);
	res->color[2] +=
		res->energy * (Amb.z + Dif.z +Spe.z);

	for (int i = 0; i < 3; i++) {
		if (res->color[i] > 1) res->color[i] = 1;
	}
	res->energy *= 0.4;
	
	glm::vec3 position = p;
	glm::vec3 direction = glm::reflect(ray.dir, n);
	CRay reflected = CRay(position, direction);
	
	if (res->energy < 0.39 || object->type == OBJ_SPHERE)
		return 0;
	else
		return reflectTrace(reflected, scene, res, object, &Amb, &Dif, &Spe);
}

int CRayTrace::reflectTrace(CRay &ray, CScene* scene, COutput* res, CObject* object, glm::vec3* amb, glm::vec3* diff, glm::vec3* spec)
{
	float t;
	object = scene->find_intersection(ray, true, &t);

	if (object == nullptr) return 0;

	int mShi = object->getShininess();
	glm::vec3 p = t*ray.dir + ray.pos;
	glm::vec3 n = object->getNormal();
	glm::vec3 v = glm::normalize(ray.pos - p);
	glm::vec3 l;
	glm::vec3 r;
	float ln;
	float vr;

	glm::vec3 mAmb = object->getAmbient();
	glm::vec3 Amb = glm::vec3(0, 0, 0);
	glm::vec3 mDif = object->getDiffuse();
	glm::vec3 Dif = glm::vec3(0, 0, 0);
	glm::vec3 mSpe = object->getSpecular();
	glm::vec3 Spe = glm::vec3(0, 0, 0);
	
	for (int i = 0; i < scene->lightCount; i++) {
		l = glm::normalize(scene->lights[i]->getPosition() - p);
		r = glm::normalize(2 * (glm::dot(n, l)) * n - l);

		CRay light = CRay(scene->lights[i]->getPosition(), glm::normalize(p - scene->lights[i]->getPosition()));

		float lightDistance = glm::length(p - scene->lights[i]->getPosition());
		float lightClosestIntersect = scene->lightTrace(light, true);
		vr = pow(glm::dot(v, r), mShi);
		if (vr < 0 || 0.99*lightDistance > lightClosestIntersect)
			vr = 0;
		ln = glm::dot(l, n);
		if (ln < 0 || 0.99*lightDistance > lightClosestIntersect)
			ln = 0;
		glm::vec3 sDif = scene->lights[i]->getDiffuse();
		Dif.x += (mDif.x * sDif.x * ln);
		Dif.y += (mDif.y * sDif.y * ln);
		Dif.z += (mDif.z * sDif.z * ln);
		glm::vec3 sSpe = scene->lights[i]->getSpecular();
		Spe.x += (mSpe.x * sSpe.x * vr);
		Spe.y += (mSpe.y * sSpe.y * vr);
		Spe.z += (mSpe.z * sSpe.z * vr);
		glm::vec3 sAmb = scene->lights[i]->getAmbient();
		Amb.x += mAmb.x * sAmb.x;
		Amb.y += mAmb.y * sAmb.y;
		Amb.z += mAmb.z * sAmb.z;
	}

	float attenuation = 1.0;

	res->color[0] +=
		res->energy * (Amb.x + Dif.x + Spe.x);
	res->color[1] +=
		res->energy * (Amb.y + Dif.y + Spe.y);
	res->color[2] +=
		res->energy * (Amb.z + Dif.z + Spe.z);

	for (int i = 0; i < 3; i++) {
		if (res->color[i] > 1) res->color[i] = 1;
	}
	res->energy *= 0.6;

	glm::vec3 position = p;
	glm::vec3 direction = glm::reflect(ray.dir, n);
	CRay reflected = CRay(position, direction);

	if (res->energy < 0.59)
		return 0;
	else
		return rayTrace(reflected, scene, res);
}

int CRayTrace::run(CScene* scene, CImage& img, bool debug) {

	glm::vec3 u = glm::normalize(glm::cross(scene->cam.up, scene->cam.lookp - scene->cam.eyep));
	glm::vec3 v = glm::normalize(glm::cross(u, scene->cam.lookp - scene->cam.eyep));
	glm::vec3 o = glm::normalize(scene->cam.lookp - scene->cam.eyep)*(float)scene->cam.width / (float)(2.0 * tan(glm::radians((float)scene->cam.FOV / 2.0))) - (float)(scene->cam.width / 2.0)*u - (float)(scene->cam.height / 2.0)*v;
	glm::mat3x3 uvo = glm::mat3x3(u, v, o);

	if (!debug) {
		for (int i = 0; i < scene->cam.width; i++)
			for (int j = 0; j < scene->cam.height; j++)
			{
				glm::vec3 xy = glm::vec3(i, j, 1);
				glm::vec3 dir = glm::normalize(uvo*xy);
				CRay ray = CRay(scene->cam.eyep, glm::normalize(uvo * glm::vec3(i, j, 1)));
				COutput output = COutput();
				rayTrace(ray, scene, &output);
				img.setPixelColor(i, j, output.color[0] * 255, output.color[1] * 255, output.color[2] * 255);

				// MSAA
				COutput outputMSAA[3];

				ray.dir = glm::normalize(uvo * glm::vec3(i + 0.5, j, 1));
				outputMSAA[0].energy = 1.0f;
				outputMSAA[0].color[0] = 0;
				outputMSAA[0].color[1] = 0;
				outputMSAA[0].color[2] = 0;
				rayTrace(ray, scene, &outputMSAA[0]);

				ray.dir = glm::normalize(uvo * glm::vec3(i + 0.5, j + 0.5, 1));
				outputMSAA[1].energy = 1.0f;
				outputMSAA[1].color[0] = 0;
				outputMSAA[1].color[1] = 0;
				outputMSAA[1].color[2] = 0;
				rayTrace(ray, scene, &outputMSAA[1]);

				ray.dir = glm::normalize(uvo * glm::vec3(i, j + 0.5, 1));
				outputMSAA[2].energy = 1.0f;
				outputMSAA[2].color[0] = 0;
				outputMSAA[2].color[1] = 0;
				outputMSAA[2].color[2] = 0;
				rayTrace(ray, scene, &outputMSAA[2]);

				output.color[0] = (output.color[0] + outputMSAA[0].color[0] + outputMSAA[1].color[0] + outputMSAA[2].color[0]) / 4;
				output.color[1] = (output.color[1] + outputMSAA[0].color[1] + outputMSAA[1].color[1] + outputMSAA[2].color[1]) / 4;
				output.color[2] = (output.color[2] + outputMSAA[0].color[2] + outputMSAA[1].color[2] + outputMSAA[2].color[2]) / 4;

				img.setPixelColor(i, j, output.color[0] * 255, output.color[1] * 255, output.color[2] * 255);
			}
	}
	else {
		cout << ">> Debug mode: ON" << endl;
		const int size = 2;
		glm::vec2 point[size];
		point[0] = glm::vec2(547, 332);
		point[1] = glm::vec2(547, 333);
		for (int i = 0; i < size; i++) {
			//INFO
			cout << endl << "---------POINT---------" << endl;
			cout << "X:    " << point[i].x << endl;
			cout << "Y:    " << point[i].y << endl;

			glm::vec3 xy = glm::vec3(point[i].x, point[i].y, 1);
			glm::vec3 dir = glm::normalize(uvo*xy);
			CRay ray = CRay(scene->cam.eyep, glm::normalize(uvo * glm::vec3(point[i].x, point[i].y, 1)));
			COutput output = COutput();
			rayTrace(ray, scene, &output);
			img.setPixelColor(point[i].x, point[i].y, output.color[0] * 255, output.color[1] * 255, output.color[2] * 255);			
		}
	}
	return 0;
}

float CScene::lightTrace(CRay & ray, bool closest_intersection)
{
	float Result = 1000000;

	for (int i = 0; i < objCount; i++)
	{
		float T = obj[i]->intersect(&ray);

		if (T < 0) continue;

		if (closest_intersection)
		{
			if (Result > T)
			{
				Result = T;
			}
		}
		else
		{
			Result = T;
		}
	}

	return Result;
}

CObject* CScene::find_intersection(CRay& ray, bool closest_intersection, float* t)
{
	*t = 1000000;
	float marginDistance = 1e-3;
	CObject* toReturn = nullptr;

	for (int i = 0; i < objCount; i++)
	{
		float T = obj[i]->intersect(&ray);

		if (T < marginDistance) continue;

		if (closest_intersection)
		{
			if (*t > T)
			{
				*t = T;
				toReturn = obj[i];
			}
		}
		else
		{
			*t = T;
			return obj[i];
		}
	}

	return toReturn;
}

void CScene::addObject(CObject* object)
{
	obj[objCount] = object;
	objCount++;
}

void CScene::addLight(CLight * light)
{
	lights[lightCount] = light;
	lightCount++;
}

CTriangle::CTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int reflect, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, int shininess, unsigned char* texture, int textureWidth, int textureHeight)
{
	this->type = OBJ_TRIANGLE;
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;
	this->normal = glm::normalize(glm::cross(p3 - p1, p2 - p1));
	this->reflect = reflect;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess;
	this->texture = texture;
	this->textureWidth = textureWidth;
	this->textureHeight = textureHeight;
}

glm::vec3 CTriangle::texturing(unsigned char * texture, int textureWidth, int textureHeight, glm::vec2 point)
{
	glm::vec3 color;

	glm::vec2 P = glm::vec2((int)(point.x * textureWidth), (int)(point.y * textureHeight));
	//zgodnie z rysunkiem z wyk³adu
	glm::vec2 A = glm::vec2(0, 0);
	glm::vec2 B = glm::vec2(textureWidth, 0); //u max
	glm::vec2 C = glm::vec2(0, textureHeight); //v max

	const unsigned int index = P.y * textureWidth * 3 + P.x * 3;
	unsigned char Red = (unsigned char)(texture[index]);
	unsigned char Green = (unsigned char)(texture[index + 1]);
	unsigned char Blue = (unsigned char)(texture[index + 2]);
	color.x = (Red / 255.0f);
	color.y = (Green / 255.0f);
	color.z = (Blue / 255.0f);

	return color;
}

float CTriangle::intersect(CRay * ray) {
	float result = -1.0f;

	glm::vec3 edge1, edge2, h, s, q;
	float a, f, u, v;

	edge1 = p2 - p1;
	edge2 = p3 - p1;

	h = glm::cross(ray->dir, edge2);
	a = glm::dot(edge1, h);
	if (a > -1e-6 && a < 1e-6)
		return result;

	f = 1 / a;
	s = ray->pos - p1;
	u = f * glm::dot(s, h);

	if (u < 0.0 || u > 1.0)
		return result;

	q = glm::cross(s, edge1);
	v = f * glm::dot(ray->dir, q);

	if (v < 0.0 || u + v > 1.0)
		return result;
	
	result = f * glm::dot(edge2, q);

	if (result <= 0.00001) 
		return result;

	this->normal = glm::normalize(glm::cross(p1 - p2, p1 - p3));
	this->color = texturing(this->texture, this->textureWidth, this->textureHeight, glm::vec2(u, v));
	this->diffuse = this->color;
	return result;
}

glm::vec3 CTriangle::setNormal(CRay * ray)
{
	return glm::cross(p3 - p1, p2 - p1);
}

glm::vec3 CTriangle::getAmbient()
{
	return this->ambient;
}

glm::vec3 CTriangle::getSpecular()
{
	return this->specular;
}

glm::vec3 CTriangle::getDiffuse()
{
	return this->diffuse;
}

glm::vec3 CTriangle::getNormal()
{
	return glm::normalize(this->normal);
}

int CTriangle::getShininess()
{
	return this->shininess;
}

CLight::CLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	this->position = position;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
}

glm::vec3 CLight::getPosition()
{
	return this->position;
}

glm::vec3 CLight::getAmbient()
{
	return this->ambient;
}

glm::vec3 CLight::getSpecular()
{
	return this->specular;
}

glm::vec3 CLight::getDiffuse()
{
	return this->diffuse;
}

COutput::COutput()
{
	this->energy = 1.0f;
	this->color[0] = 0.0f;
	this->color[1] = 0.0f;
	this->color[2] = 0.0f;
}

CRay::CRay(glm::vec3 pos, glm::vec3 dir)
{
	this->pos = pos;
	this->dir = dir;
}
