/*
Práctica 7: Iluminación 1 
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList; 

GLfloat r_Car;
GLfloat g_Car;
GLfloat b_Car;


Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture DadoEmo_T;
Texture dice_eight;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;
Model Carro_M;
Model Carro_PDI, Carro_PDD, Carro_PTD, Carro_PTI, Carro_Cofre;
Model LampPost;
Model Pecera;
Model antena_P, foco_P, Pez;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
PointLight temp;
SpotLight spotLights[MAX_SPOT_LIGHTS];


// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


void CrearDado()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,

		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,

		// right
		4, 5, 6,
		6, 7, 4,

	};	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.26f,  0.34f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.49f,	0.34f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.49f,	0.66f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.26f,	0.66f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.0f,  0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	1.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.0f,  0.0f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		1.0f,	0.0f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f,	1.0f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,  0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	1.0f,	0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	1.0f,	1.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	1.0f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(dado);

}


void CrearPiramide_UP()
{
	unsigned int piramide_indices[] = {
		// Base (2 triangles)
		0, 1, 2,
		2, 3, 0,

		// Front face
		4, 5, 6,
		// Right face
		7, 8, 9,
		// Back face
		10, 11, 12,
		// Left face
		13, 14, 15
	};

	GLfloat piramide_vertices[] = {
		// x      y      z      S      T       NX     NY     NZ

		// --- BASE (Bottom) --- Normal: [0, -1, 0]
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,   0.0f, -1.0f,  0.0f, // 0
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,   0.0f, -1.0f,  0.0f, // 1
		 0.5f, -0.5f, -0.5f,  1.0f,  1.0f,   0.0f, -1.0f,  0.0f, // 2
		-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,   0.0f, -1.0f,  0.0f, // 3

		// --- FRONT FACE --- Normal: [0, 0.5, 0.8] approx
		-0.5f, -0.5f,  0.5f,  0.021f,  0.6511f,   0.0f,  0.44f, -0.89f, // 4
		 0.5f, -0.5f,  0.5f,  0.21f,  0.6511f,   0.0f,  0.44f, -0.89f, // 5
		 0.0f,  0.5f,  0.0f,  0.13f,  0.988f,   0.0f,  0.44f, -0.89f, // 6 (Apex)

		 // --- RIGHT FACE --- Normal: [0.8, 0.5, 0] approx
		 0.5f, -0.5f,  0.5f,  0.267f,  0.6511f,   -0.89f, 0.44f, 0.0f,  // 7
		 0.5f, -0.5f, -0.5f,  0.464f,  0.6511f,   -0.89f, 0.44f, 0.0f,  // 8
		 0.0f,  0.5f,  0.0f,  0.36f,  0.988f,   -0.89f, 0.44f, 0.0f,  // 9 (Apex)

		 // --- BACK FACE --- Normal: [0, 0.5, -0.8] approx
		 0.5f, -0.5f, -0.5f,  0.519f,  0.6511f,   0.0f,  0.44f, 0.89f, // 10
		 -0.5f, -0.5f, -0.5f,  0.71f,  0.6511f,   0.0f,  0.44f, 0.89f, // 11
		 0.0f,  0.5f,  0.0f,  0.606f,  0.988f,   0.0f,  0.44f, 0.89f, // 12 (Apex)

		 // --- LEFT FACE --- Normal: [-0.8, 0.5, 0] approx
		 -0.5f, -0.5f, -0.5f,  0.776f,  0.6511f,  0.89f, 0.44f, 0.0f,  // 13
		 -0.5f, -0.5f,  0.5f,  0.945f,  0.6511f,  0.89f, 0.44f, 0.0f,  // 14
		  0.0f,  0.5f,  0.0f,  0.841f,  0.988f,  0.89f, 0.44f, 0.0f   // 15 (Apex)
	};

	Mesh* piramide = new Mesh();
	// 16 vertices * 8 floats per vertex = 128
	// 18 indices (6 for base, 3 per side * 4 sides)
	piramide->CreateMesh(piramide_vertices, piramide_indices, 128, 18);
	meshList.push_back(piramide);
}

void CrearPiramide_DOWN()
{
	unsigned int piramide_indices[] = {
		// Base (2 triangles)
		0, 1, 2,
		2, 3, 0,

		// Front face
		4, 5, 6,
		// Right face
		7, 8, 9,
		// Back face
		10, 11, 12,
		// Left face
		13, 14, 15
	};

	GLfloat piramide_vertices[] = {
		// x      y      z      S      T       NX     NY     NZ

		// --- BASE (Bottom) --- Normal: [0, -1, 0]
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,   0.0f, -1.0f,  0.0f, // 0
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,   0.0f, -1.0f,  0.0f, // 1
		 0.5f, -0.5f, -0.5f,  1.0f,  1.0f,   0.0f, -1.0f,  0.0f, // 2
		-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,   0.0f, -1.0f,  0.0f, // 3

		// --- FRONT FACE --- Normal: [0, 0.5, 0.8] approx
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0465f,   0.0f,  0.44f, -0.89f, // 4
		 0.5f, -0.5f,  0.5f,  0.196f,  0.0465f,   0.0f,  0.44f, -0.89f, // 5
		 0.0f,  0.5f,  0.0f,  0.103f,  0.384f,   0.0f,  0.44f, -0.89f, // 6 (Apex)

		 // --- RIGHT FACE --- Normal: [0.8, 0.5, 0] approx
		 0.5f, -0.5f,  0.5f,  0.267f,  0.0465f,  -0.89f, 0.44f, 0.0f,  // 7
		 0.5f, -0.5f, -0.5f,  0.464f,  0.0465f,   -0.89f, 0.44f, 0.0f,  // 8
		 0.0f,  0.5f,  0.0f,  0.36f,  0.384f,   -0.89f, 0.44f, 0.0f,  // 9 (Apex)

		 // --- BACK FACE --- Normal: [0, 0.5, -0.8] approx
		 0.5f, -0.5f, -0.5f,  0.540f,  0.0232f,   0.0f,  0.44f, 0.89f, // 10
		 -0.5f, -0.5f, -0.5f,  0.743f,  0.0465f,   0.0f,  0.44f, 0.89f, // 11
		 0.0f,  0.5f,  0.0f,  0.639f,  0.372f,   0.0f,  0.44f, 0.89f, // 12 (Apex)

		 // --- LEFT FACE --- Normal: [-0.8, 0.5, 0] approx
		 -0.5f, -0.5f, -0.5f,  0.792f,  0.0581f, 0.89f, 0.44f, 0.0f,  // 13
		 -0.5f, -0.5f,  0.5f,  0.989f,  0.093f,  0.89f, 0.44f, 0.0f,  // 14
		  0.0f,  0.5f,  0.0f,  0.885f,  0.441f,  0.89f, 0.44f, 0.0f   // 15 (Apex)
	};

	Mesh* piramide = new Mesh();
	// 16 vertices * 8 floats per vertex = 128
	// 18 indices (6 for base, 3 per side * 4 sides)
	piramide->CreateMesh(piramide_vertices, piramide_indices, 128, 18);
	meshList.push_back(piramide);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	CreateObjects();
	CrearDado();
	CreateShaders();
	CrearPiramide_UP();
	CrearPiramide_DOWN();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	DadoEmo_T = Texture("Textures/Cubo_Emociones.tga");
	DadoEmo_T.LoadTextureA();
	dice_eight = Texture("Textures/8sideddice.tga");
	dice_eight.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Carro_M = Model();
	Carro_M.LoadModel("Models/Buggati.obj");
	Carro_PDD = Model();
	Carro_PDD.LoadModel("Models/Llanta_DD.obj");
	Carro_PTD = Model();
	Carro_PTD.LoadModel("Models/Llanta_DT.obj");
	Carro_PDI = Model();
	Carro_PDI.LoadModel("Models/Llanta_ID.obj");
	Carro_PTI = Model();
	Carro_PTI.LoadModel("Models/Llanta_IT.obj");
	Carro_Cofre = Model();
	Carro_Cofre.LoadModel("Models/Cofre.obj");
	LampPost = Model();
	LampPost.LoadModel("Models/LampPost.obj");
	Pecera = Model();
	Pecera.LoadModel("Models/12985_Fish_bowl_v1_l1.obj");
	antena_P = Model();
	antena_P.LoadModel("Models/antena_pez.obj");
	foco_P = Model();
	foco_P.LoadModel("Models/foco_pez.obj");
	Pez = Model();
	Pez.LoadModel("Models/cuerpo_pez.obj");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.1f, 0.1,
		0.0f, -1.0f, 0.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		1.0f, 0.1f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;


	// luz de la lampara
	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
		5.0f, 0.1f,
		20.5f, 8.0f, 7.0f,
		1.0f, 0.3f, 0.02f
	);
	pointLightCount++;

	// luz del pez
	pointLights[2] = PointLight(0.0f, 0.0f, 1.0f,
		5.0f, 0.1f,
		30.5f, 8.0f + mainWindow.gety_Pez(), 13.0f + mainWindow.getz_Pez(),
		1.0f, 0.3f, 0.02f
	);
	pointLightCount++;


	unsigned int spotLightCount = 0;

	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	// faro del helicóptero (adelante)
	spotLights[1] = SpotLight(1.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		1.0f, -5.0f, 0.0f,
		0.5f, 0.0f, 0.0f,
		25.0f);
	spotLightCount++;

	// faro del coche
	/*
	spotLights[2] = SpotLight(r_Car, g_Car, b_Car,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;
	*/

	// faro del pez
	spotLights[2] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		30.5f, 8.0f + mainWindow.gety_Pez(), 13.0f + mainWindow.getz_Pez(),
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;

	// luz del helicóptero (trasera)
	spotLights[3] = SpotLight(0.0f, 1.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		-1.0f, -5.0f, 0.0f,
		0.5f, 0.0f, 0.0f,
		25.0f);
	spotLightCount++;

	


	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	glm::vec3 lowerLight = camera.getCameraPosition();
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		//spotLights[1].SetPos(poscoche + glm::vec(x, y, cofre));

		// luz amarilla
		glm::vec3 heliLight_front = glm::vec3(0.0f + mainWindow.getmuevex_heli(), 5.0f, 6.0f);
		spotLights[1].SetFlash(heliLight_front, glm::vec3(-1.0f, -1.0f, 0.0f));
		glm::vec3 heliLight_back = glm::vec3(0.0f + mainWindow.getmuevex_heli(), 5.0f, 6.0f);
		spotLights[3].SetFlash(heliLight_front, glm::vec3(1.0f, -1.0f, 0.0f));

		/*
		if (mainWindow.getcolorFaro() == 0) {
			spotLights[2].SetLight(1.0f, 0.0f, 0.0f);
		}
		else if (mainWindow.getcolorFaro() == 1) {
			spotLights[2].SetLight(1.0f, 1.0f, 0.0f);
		}
		else if (mainWindow.getcolorFaro() == 2) {
			spotLights[2].SetLight(0.0f, 1.0f, 0.0f);
		}
		else if (mainWindow.getcolorFaro() == 3) {
			spotLights[2].SetLight(0.0f, 1.0f, 1.0f);
		}
		else if (mainWindow.getcolorFaro() == 4) {
			spotLights[2].SetLight(0.0f, 0.0f, 1.0f);
		}
		else if (mainWindow.getcolorFaro() == 5) {
			spotLights[2].SetLight(1.0f, 0.0f, 1.0f);
		}
		*/

		if (mainWindow.getheli_dir() == 1) {
			spotLights[1].SetLight(0.0f, 0.0f, 0.0f);
			spotLights[3].SetLight(0.0f, 1.0f, 1.0f);
		}
		else {
			spotLights[3].SetLight(0.0f, 0.0f, 0.0f);
			spotLights[1].SetLight(1.0f, 1.0f, 0.0f);
		}

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		if (mainWindow.getapagarLampara() == 0 && mainWindow.getapagarLampara2() == 0) {
			shaderList[0].SetPointLights(pointLights, pointLightCount);
		}
		else if (mainWindow.getapagarLampara() == 1 && mainWindow.getapagarLampara2() == 0) {
			temp = PointLight(1.0f, 1.0f, 1.0f,
				5.0f, 0.1f,
				20.5f, 8.0f, 7.0f,
				1.0f, 0.3f, 0.02f
			);
			pointLights[1] = temp;
			shaderList[0].SetPointLights(pointLights, pointLightCount-1);
		}
		else if (mainWindow.getapagarLampara() == 0 && mainWindow.getapagarLampara2() == 1) {
			temp = PointLight(0.0f, 0.0f, 1.0f,
				5.0f, 0.1f,
				30.5f, 8.0f + mainWindow.gety_Pez(), 13.0f + mainWindow.getz_Pez(),
				1.0f, 0.3f, 0.02f
			);
			pointLights[1] = temp;

			shaderList[0].SetPointLights(pointLights, pointLightCount-1);
		}
		else if (mainWindow.getapagarLampara() == 1 && mainWindow.getapagarLampara2() == 1) {
			shaderList[0].SetPointLights(pointLights, pointLightCount - 2);
		}
		shaderList[0].SetSpotLights(spotLights, spotLightCount);



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		/*
		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), 0.5f, -3.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kitt_M.RenderModel();

		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);//llanta con color gris
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();
		*/
	
		// mi coche
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), -2.0f, -7.5f));
		model = glm::rotate(model, glm::radians(-100.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_M.RenderModel();
		model = modelaux;

		// luz azul
		glm::vec3 carLight = glm::vec3(0.0f + mainWindow.getmuevex(), -2.0f, -7.5f);
		carLight.x += 1.5f;
		carLight.y += 0.75f;
		carLight.z -= 3.5f;
		spotLights[3].SetFlash(carLight, glm::vec3(-1.0f, 0.0f, 0.0f));

		// Llanta derecha frontal
		model = glm::translate(model, glm::vec3(-0.3f, 0.3f, -0.4f));
		model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_PDD.RenderModel();
		model = modelaux;

		// Llanta izquierda frontal
		model = glm::translate(model, glm::vec3(1.3f, -0.75f, 2.1f));
		model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_PDI.RenderModel();
		model = modelaux;

		// Llanta derecha trasera
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, -1.2f));
		model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_PTD.RenderModel();
		model = modelaux;

		// Llanta izquierda trasera
		model = glm::translate(model, glm::vec3(0.8f, 0.25f, -0.0f));
		model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		//model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_PTI.RenderModel();
		model = modelaux;

		// cofre
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::scale(model, glm::vec3(30.0f, 30.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_Cofre.RenderModel();

		// Helicóptero con movimiento
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex_heli(), 5.0f, 6.0));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();
		model = modelaux;

		
		
		

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 5.0f, 6.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		meshList[4]->RenderMesh();

		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		DadoEmo_T.UseTexture();
		meshList[4]->RenderMesh();

		// programación del faro
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.5f, -0.75f, 7.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LampPost.RenderModel();
		model = modelaux;
		
		// dado de 8 caras
		model = glm::mat4(1.0);

		model = glm::translate(model, glm::vec3(-0.0f, 20.0f, 3.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(4.0f, 4.00f, 4.00f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dice_eight.UseTexture();
		meshList[5]->RenderMesh();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.0f, -4.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.00f, 4.00f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dice_eight.UseTexture();
		meshList[6]->RenderMesh();

		// pecera
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(30.0f, -1.25f, 15.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pecera.RenderModel();
		glDisable(GL_BLEND);

		//pez
		model = glm::translate(model, glm::vec3(0.0f, 4.0f + mainWindow.gety_Pez(), 7.5f - mainWindow.gety_Pez()));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pez.RenderModel();

		model = glm::translate(model, glm::vec3(1.25f, 6.5f, -2.4f));
		//model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		antena_P.RenderModel();

		model = glm::translate(model, glm::vec3(0.0f, 9.05f, 6.5f));
		//model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		foco_P.RenderModel();

		glm::vec3 fishLight = glm::vec3(30.5f, 8.0f + mainWindow.gety_Pez(), 13.0f + mainWindow.getz_Pez());
		spotLights[2].SetFlash(fishLight, glm::vec3(mainWindow.getX(), mainWindow.getY(), mainWindow.getZ()));


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
