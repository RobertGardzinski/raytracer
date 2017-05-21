#include <iostream>
#include "FreeImage.h"
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/glm.hpp>

using namespace std;

// Klasa opisujaca promien
class CRay{
public:
	glm::vec3 pos;
	glm::vec3 dir;

	CRay(glm::vec3 pos, glm::vec3 dir);
};

// Klasa opisujaca parametry kamery
class CCamera {
public:
	int width = 1000;
	int height = 800;
	int FOV = 50;
	glm::vec3 eyep = glm::vec3(3.0f, 15.0f, -16.0f);
	glm::vec3 lookp = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	//glm::vec3 eyep = glm::vec3(0.0f, 2.0f, -10.0f);
	//glm::vec3 lookp = glm::vec3(0.0f, 3.0f, 7.0f);
	//glm::vec3 up = glm::vec3(0.0f, 1.0f, 3.0f);
};

// Wirtualna klasa definujÄ?ca obiekt
class CObject {
public:
	int type; /**< rodzaj obiektu (OBJ_SPHERE, OBJ_TRIANGLE) */
	
	CObject();
	
	/** Obliczenie przeciÄ?cia promienia z obiektem */
	virtual float intersect(CRay* ray) = 0;
	virtual glm::vec3 setNormal(CRay* ray) = 0;
	virtual glm::vec3 getAmbient() = 0;
	virtual glm::vec3 getSpecular() = 0;
	virtual glm::vec3 getDiffuse() = 0;
	virtual glm::vec3 getNormal() = 0;
	virtual int getShininess() = 0;

};

// Klasa opisujaca obiekt - kula
class CSphere : public CObject {
private:
	float r; /**< promien */
	glm::vec3 center; /* wektor z polozeniem srodka */
	int reflect;
	glm::vec3 ambient; 
	glm::vec3 diffuse;
	glm::vec3 specular;
	int shininess;
	glm::vec3 normal;
	unsigned char* texture;	
	int textureWidth;
	int textureHeight;
	glm::vec3 color;
		
public:
	CSphere(float r, glm::vec3 center, int reflect, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, int shininess, unsigned char* texture, int textureWidth, int textureHeight);
	
	glm::vec3 texturing(unsigned char* texture, int textureWidth, int textureHeight, glm::vec3 point);
	/** Obliczenie przeciÄ?cia promienia z kula */
	float intersect(CRay* ray);
	glm::vec3 setNormal(CRay* ray);
	glm::vec3 getAmbient();
	glm::vec3 getSpecular();
	glm::vec3 getDiffuse();
	glm::vec3 getNormal();
	int getShininess();
};

// Klasa opisujaca obiekt - triangle
class CTriangle : public CObject {
public:
	glm::vec3 p1;
	glm::vec3 p2;
	glm::vec3 p3;
	int reflect;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	int shininess;
	glm::vec3 normal;
	unsigned char* texture;
	int textureWidth;
	int textureHeight;
	glm::vec3 color;
	
	CTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int reflect, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, int shininess, unsigned char* texture, int textureWidth, int textureHeight);

	glm::vec3 texturing(unsigned char* texture, int textureWidth, int textureHeight, glm::vec2 point);
	float intersect(CRay* ray);
	/** Obliczenie przeciÄ?cia promienia z kula */
	//float intersect(CRay* ray);
	glm::vec3 setNormal(CRay* ray);
	glm::vec3 getAmbient();
	glm::vec3 getSpecular();
	glm::vec3 getDiffuse();
	glm::vec3 getNormal();
	int getShininess();
};

// Definicja zrodla swiatla
class CLight  {
private:
	glm::vec3 position; /**< polozenie srodka */
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
public:
	CLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	glm::vec3 getPosition();
	glm::vec3 getAmbient();
	glm::vec3 getSpecular();
	glm::vec3 getDiffuse();
};

// Klasa opisujaca scene, definicje kamery, zrodel swiatla, obiektow, itp.
class CScene {
public:
	CCamera cam; /**< kamera */

	CObject* obj[200]; /**< lista z definicjami obiektow */
	int objCount = 0; /**< liczba obiektow na liscie */
	
	CLight* lights[10];
	int lightCount = 0;

	void addObject(CObject* object);
	void addLight(CLight *light);
	
	/** Parsowanie pliku tektowego z informacjami o scenie.
	 *	@param - nazwa pliku
	 */
	int parse(char* fname);

	CObject* find_intersection(CRay& ray, bool closest_intersection, float *t);

	float lightTrace(CRay& ray, bool closest_intersection);
};

// Klasa reprezentujaca obraz wyjsciowy
class CImage {
private:
	float* data; /**< dane obrazu */
	int width; /**< liczba pikseli w poziomie */
	int height; /**< liczba pikseli w pionie */
	int BPP; /**< liczba bitów na piksel */
	FIBITMAP* bitmap;
	RGBQUAD color;	
public:
	int start;

	CImage(int aWidth, int aHeight, int aBPP);

	/** Przygotowanie bitmapy wype³nionej czerwonymi pikselami
	*/
	void init();

	/** Zapisanie czerwonej bitmapy na dysku
	*/
	void save();

	void setPixelColor(int i, int j, int R, int G, int B);

	glm::vec3 getPixelColor(int i, int j);

	/** Zapisanie obrazu w pliku dyskowym
	 *	@param fname - nazwa pliku
	 */
	//void save(char* fname);
	
};

// Dane przekazywane w procedurze rekurencyjnej
class COutput {
public:
	float energy;
	int tree;
	float color[3];

	COutput();
};

// Glowna klasa ray tracera
class CRayTrace {
	
public:
	bool debug = true;
	
	
	/** Sledzenie promienia
	 */
	int rayTrace(CRay &ray, CScene* scene, COutput* res);
	int reflectTrace(CRay &ray, CScene* scene, COutput* res, CObject* object, glm::vec3* amb, glm::vec3* diff, glm::vec3* spec);
	
	
	/** Glowna petla ray tracera
	 */
	int run(CScene* scene, CImage& img, bool debug);

};