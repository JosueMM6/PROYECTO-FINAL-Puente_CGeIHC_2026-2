/*---------------------------------------------------------*/
/* ----------------   PROYECTO FINAL - CGeIHC --------------------------*/
/*-----------------    2026-2   -------------------------------*/
/*------------- Alumno: Garcia Vazquez Javier Alejandro   ---------*/
/*------------- No. Cuenta: 319129793      -------------------*/
/*------------- Alumno: Marin Montaño Josue   ---------------*/
/*------------- No. Cuenta: 319235630    ---------------*/


#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>						//main
#include <stdlib.h>		
#include <glm/glm.hpp>					//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>					//Texture

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>
#include <mmsystem.h>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;

GLuint VBO[3], VAO[3], EBO[3];

//Camera
Camera camera(glm::vec3(0.0f, 2.0f, 20.0f));
float MovementSpeed = 0.1f;
GLfloat lastX = SCR_WIDTH / 2.0f,
lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//Timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
lastFrame = 0.0f;

void getResolution(void);
void myData(void);							// De la practica 4
void LoadTextures(void);					// De la práctica 6
unsigned int generateTextures(char*, bool, bool);	// De la práctica 6

//For Keyboard
float	movX = 0.0f,
movY = 0.0f,
movZ = -5.0f,
rotX = 0.0f;

//Texture
unsigned int	t_smile,
t_toalla,
t_unam,
t_white,
t_ladrillos;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

//// Light
glm::vec3 lightColor = glm::vec3(0.7f);
glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

// posiciones
int estadoC1_walking = 0;
float	movAuto_x = 0.0f,
movAuto_z = 0.0f,
orienta = 90.0f;
bool	animacion = false,
recorrido1 = true,
recorrido2 = false,
recorrido3 = false,
recorrido4 = false;


//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
posY = 0.0f,
posZ = 0.0f,
rotRodIzq = 0.0f,
giroMonito = 0.0f,
giroCabeza = 0.0f,
giroBrazoDer = 0.0f,
giroBrazoIzq = 0.0f,
giroPiernaDer = 0.0f,
giroPiernaIzq = 0.0f;

//-----------------------------------------------------------------------------------------------------
// PF - Rocket
//-----------------------------------------------------------------------------------------------------
int stateRocket = 0;
float	turnRocket = 0.0f,
turnBase = 0.0f,
movRocket_x = 0.0f,
movRocket_y = 0.0f,
movRocket_z = 0.0f,
movS1 = 0.0f,
movS2 = 0.0f,
movS3 = 0.0f;
bool	aniRocket = false;

//-----------------------------------------------------------------------------------------------------
// Personaje de Fondo - C1 Walking
//-----------------------------------------------------------------------------------------------------
float movC1_x = -128.0f,
	movC1_y = 58.0f,
	movC1_z = -100.0f;


float	incX = 0.0f,
incY = 0.0f,
incZ = 0.0f,
rotRodIzqInc = 0.0f,
giroMonitoInc = 0.0f,
giroCabezaInc = 0.0f,
giroBrazoDerInc = 0.0f,
giroBrazoIzqInc = 0.0f,
giroPiernaDerInc = 0.0f,
giroPiernaIzqInc = 0.0f;

//con em MAX FRAMES son la cantidad de frames para generar la animacion. El detalle está en que consumirá memoria al guardar los datos para animar.
#define MAX_FRAMES 9
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;
	float giroCabeza;
	float giroBrazoDer;
	float giroBrazoIzq;
	float giroPiernaDer;
	float giroPiernaIzq;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir número en caso de tener Key guardados
//int FrameIndex = 3;			//Si ya tenemos frames guardados, introducimos el número de frames que tenemos para seguir guardando a partir de ahí. En este caso, como ya tengo 3 frames guardados, introduzco el número 3 para seguir guardando a partir de ahí.
bool play = false;
int playIndex = 0;

//Para guardar un arreglo con los frames que forman parte de la animación
void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;
	KeyFrame[FrameIndex].giroCabeza = giroCabeza;
	KeyFrame[FrameIndex].giroBrazoDer = giroBrazoDer;
	KeyFrame[FrameIndex].giroBrazoIzq = giroBrazoIzq;
	KeyFrame[FrameIndex].giroPiernaDer = giroPiernaDer;
	KeyFrame[FrameIndex].giroPiernaIzq = giroPiernaIzq;

	std::cout << "PosX = " << posX << std::endl;
	std::cout << "PosY = " << posY << std::endl;
	std::cout << "PosZ = " << posZ << std::endl;

	FrameIndex++;
}

//Para empezar la animación desde el primer frame que se guardó
void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;
	giroCabeza = KeyFrame[0].giroCabeza;
	giroBrazoDer = KeyFrame[0].giroBrazoDer;
	giroBrazoIzq = KeyFrame[0].giroBrazoIzq;
	giroPiernaDer = KeyFrame[0].giroPiernaDer;
	giroPiernaIzq = KeyFrame[0].giroPiernaIzq;

}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotRodIzqInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;
	giroCabezaInc = (KeyFrame[playIndex + 1].giroCabeza - KeyFrame[playIndex].giroCabeza) / i_max_steps;
	giroBrazoDerInc = (KeyFrame[playIndex + 1].giroBrazoDer - KeyFrame[playIndex].giroBrazoDer) / i_max_steps;
	giroBrazoIzqInc = (KeyFrame[playIndex + 1].giroBrazoIzq - KeyFrame[playIndex].giroBrazoIzq) / i_max_steps;
	giroPiernaDerInc = (KeyFrame[playIndex + 1].giroPiernaDer - KeyFrame[playIndex].giroPiernaDer) / i_max_steps;
	giroPiernaIzqInc = (KeyFrame[playIndex + 1].giroPiernaIzq - KeyFrame[playIndex].giroPiernaIzq) / i_max_steps;
}

unsigned int generateTextures(const char* filename, bool alfa, bool isPrimitive)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;

	if (isPrimitive)
		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	else
		stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.


	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 100;
	}

	stbi_image_free(data);
}

void LoadTextures()
{

	t_smile = generateTextures("Texturas/awesomeface.png", 1, true);
	t_toalla = generateTextures("Texturas/toalla.tga", 0, true);
	t_unam = generateTextures("Texturas/escudo_unam.jpg", 0, true);
	t_ladrillos = generateTextures("Texturas/bricks.jpg", 0, true);
	//This must be the last
	t_white = generateTextures("Texturas/white.jpg", 0, false);
}



void animate(void)
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rotRodIzqInc;
			giroMonito += giroMonitoInc;
			giroCabeza += giroCabezaInc;
			giroBrazoDer += giroBrazoDerInc;
			giroBrazoIzq += giroBrazoIzqInc;
			giroPiernaDer += giroPiernaDerInc;
			giroPiernaIzq += giroPiernaIzqInc;

			i_curr_steps++;
		}

	}


	//-----------------------------------------------------------------------------------------------------
	// Personaje de Fondo - C1 Walking
	//-----------------------------------------------------------------------------------------------------
	if (animacion) {
		if (estadoC1_walking == 1) // Retrocede hasta -200 en x
		{
			movC1_z += 1.0f;
			if (movC1_z > 300.0f)
				estadoC1_walking = 0;
		}
	}

	//-----------------------------------------------------------------------------------------------------
	// PF - Rocket
	//-----------------------------------------------------------------------------------------------------
	if (aniRocket)
	{
		if (stateRocket == 1) { //Gira la base 150
			turnBase += 0.35f;
			if (turnBase > 150.0f) {
				stateRocket = 2;
			}
		}
		else if (stateRocket == 2) { //Abren los soportes
			movS1 += 0.35;
			movS2 += 0.35;
			movS3 += 0.35;
			if (movS1 > 20.0f && movS2 > 20.0f && movS3 > 20.0f) {
				stateRocket = 3;
			}
		}
		else if (stateRocket == 3) { // Despegue
			movRocket_y += 0.4f;

			// Genera una curva
			if (movRocket_y > 30.0f) {
				movRocket_z += (movRocket_y * 0.002f); //Acelera horizontalmente según sube
			}
			// Para generar una inclinacion gradual conforme sube
			float ascent = glm::clamp(movRocket_y / 250.0f, 0.0f, 1.0f);

			//El cohete se inclina suavemente
			turnRocket = ascent * 60.0f;

			if (movRocket_y > 250.0f) {
				stateRocket = 4;
			}
		}
		else if (stateRocket == 4) { // Descenso
			movRocket_y -= 0.4f;
			movRocket_z += 0.8f;

			//El progreso es inverso a la altura
			float descent = glm::clamp(movRocket_y / 250.0f, 0.0f, 1.0f);
			turnRocket = descent * 60.0f;

			if (movRocket_y < 0.0f) {
				stateRocket = 0;
			}
		}
	}

}

void getResolution() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

void myData() {
	float vertices[] = {
		// positions          // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	float verticesPiso[] = {
		// positions          // texture coords
		 10.5f,  10.5f, 0.0f,   4.0f, 4.0f, // top right
		 10.5f, -10.5f, 0.0f,   4.0f, 0.0f, // bottom right
		-10.5f, -10.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-10.5f,  10.5f, 0.0f,   0.0f, 4.0f  // top left 
	};
	unsigned int indicesPiso[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	GLfloat verticesCubo[] = {
		//Position				//texture coords
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0 - Frontal
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5

		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2 - Trasera
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,	//V3
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Izq
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, -0.5f, 0.5f,		0.0f, 1.0f,	//V0

		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5 - Der
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5
		0.5f, 0.5f, -0.5f,		1.0f, 0.0f,	//V6
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Sup
		0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V5
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6

		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0 - Inf
		-0.5f, -0.5f, -0.5f,	1.0f, 1.0f,	//V3
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V1
	};

	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(3, EBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Para Piso
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPiso), verticesPiso, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPiso), indicesPiso, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	//PARA CUBO
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubo), verticesCubo, GL_STATIC_DRAW);

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main() {
	// glfw: initialize and configure
	glfwInit();

	// glfw window creation
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Practica 4 2026-2", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	myData();
	glEnable(GL_DEPTH_TEST);



	// build and compile shaders
	// -------------------------
	Shader myShader("shaders/shader_texture_color.vs", "shaders/shader_texture_color.fs"); //To use with primitives
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");	//To use with static models
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");	//To use with skybox
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");	//To use with animated models 

	vector<std::string> faces{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
	Model piso("resources/objects/piso/piso.obj");
	Model puente("resources/objects/Puente/Puente.obj");

	// Modelo del Windmill descargado como actividad complementaria a la práctica 4
	Model windMill("resources/objects/Windmill/Windmill_P4.obj");

	ModelAnim animacionPersonaje("resources/objects/Personaje1/Arm.dae");
	animacionPersonaje.initShaders(animShader.ID);


	//-----------------------------------------------------------------------------------------------------
	// Personajes de fondo
	//-----------------------------------------------------------------------------------------------------
	ModelAnim C1_Walking("resources/objects/PersonajesFondo/C1_Walking/C1_Walking.dae");
	C1_Walking.initShaders(animShader.ID);

	ModelAnim C2_LeftStrafeWalking("resources/objects/PersonajesFondo/C2_LeftStrafeWalking/C2_LeftStrafeWalking.dae");
	C2_LeftStrafeWalking.initShaders(animShader.ID);



	Model drone("resources/objects/Drone/Drone_v1.obj");

	//-----------------------------------------------------------------------------------------------------
	// PF - Rocket
	//-----------------------------------------------------------------------------------------------------
	Model baseRocket("resources/objects/Rocket/Base.obj");
	Model rocket("resources/objects/Rocket/Rocket.obj");
	Model support1("resources/objects/Rocket/Support1.obj");
	Model support2("resources/objects/Rocket/Support2.obj");
	Model support3("resources/objects/Rocket/Support3.obj");

	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
		KeyFrame[i].giroCabeza = 0;
		KeyFrame[i].giroBrazoDer = 0;
		KeyFrame[i].giroBrazoIzq = 0;
		KeyFrame[i].giroPiernaDer = 0;
		KeyFrame[i].giroPiernaIzq = 0;
	}

	/*
		KeyFrame[0].posX = 0;
		KeyFrame[0].posY = 0;
		KeyFrame[0].posZ = 0;
		KeyFrame[0].rotRodIzq = 0;
		KeyFrame[0].giroMonito = 0;
		KeyFrame[0].giroCabeza = 0;
		KeyFrame[0].giroBrazoDer = 0;}

		KeyFrame[1].posX = 0;
		KeyFrame[1].posY = 0;
		KeyFrame[1].posZ = 0;
		KeyFrame[1].rotRodIzq = 0;
		KeyFrame[1].giroMonito = 0;
		KeyFrame[1].giroCabeza = 0;
		KeyFrame[1].giroBrazoDer = 0;

		KeyFrame[2].posX = 0;
		KeyFrame[2].posY = 0;
		KeyFrame[2].posZ = 0;
		KeyFrame[2].rotRodIzq = 0;
		KeyFrame[2].giroMonito = 0;
		KeyFrame[2].giroCabeza = 0;
		KeyFrame[2].giroBrazoDer = 0;
	*/


	// create transformations and Projection
	glm::mat4 modelOp = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
	glm::mat4 viewOp = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projectionOp = glm::mat4(1.0f);	//This matrix is for Projection

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		//Setup shader for static models
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", ambientColor);
		staticShader.setVec3("dirLight.diffuse", diffuseColor);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.6f, 0.6f, 0.6f));

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setVec3("spotLight[0].position", glm::vec3(0.0f, 20.0f, 10.0f));
		staticShader.setVec3("spotLight[0].direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(60.0f)));
		staticShader.setFloat("spotLight[0].constant", 1.0f);
		staticShader.setFloat("spotLight[0].linear", 0.0009f);
		staticShader.setFloat("spotLight[0].quadratic", 0.0005f);

		staticShader.setFloat("material_shininess", 32.0f);

		//glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		viewOp = camera.GetViewMatrix();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		//Setup shader for primitives
		myShader.use();
		// view/projection transformations
		//projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 400.0f);
		viewOp = camera.GetViewMatrix();
		// pass them to the shaders
		//myShader.setMat4("model", modelOp);
		myShader.setMat4("view", viewOp);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		myShader.setMat4("projection", projectionOp);
		/**********/

		skyboxShader.use();
		skybox.Draw(skyboxShader, viewOp, projectionOp, camera);

		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projectionOp);
		animShader.setMat4("view", viewOp);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-40.3f, 1.75f, 0.3f)); // translate it down so it's at the center of the scene
		modelOp = glm::scale(modelOp, glm::vec3(0.05f));	// it's a bit too big for our scene, so scale it down
		modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", modelOp);
		animacionPersonaje.Draw(animShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// C1 wey caminando en linea recta
		// -------------------------------------------------------------------------------------------------------------------------
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(movC1_x, movC1_y, movC1_z));
		modelOp = glm::scale(modelOp, glm::vec3(0.07f));	// it's a bit too big for our scene, so scale it down
		animShader.setMat4("model", modelOp);
		C1_Walking.Draw(animShader);




		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------
		myShader.use();

		//Tener Piso como referencia
		glBindVertexArray(VAO[2]);
		//Colocar código aquí
		modelOp = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f, 2.0f, 40.0f));
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_ladrillos);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(VAO[0]);
		//Colocar código aquí
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(5.0f, 5.0f, 1.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_unam);
		//glDrawArrays(GL_TRIANGLES, 0, 36); //A lonely cube :(
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		
		glBindVertexArray(0);
		// ------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

	

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.75f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.2f));
		staticShader.setMat4("model", modelOp);
		piso.Draw(staticShader);

		
		modelOp = glm::mat4(1.0f);
		// Posicionar (Ajusta el segundo valor para la altura sobre los ladrillos)
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, 0.0f, 0.0f));
		// Rotar (Si es necesario orientarlo)
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		// ESCALAR (Modifica estos valores para que abarque la base de 40x40)
		// Si el puente es pequeño, prueba con valores como 5.0f o 10.0f
		modelOp = glm::scale(modelOp, glm::vec3(7.0f, 7.0f, 7.0f));

		staticShader.setMat4("model", modelOp);
		puente.Draw(staticShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// DRONE
		// -------------------------------------------------------------------------------------------------------------------------
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-40.3f, 20.75f, 0.3f)); // translate it down so it's at the center of the scene
		modelOp = glm::scale(modelOp, glm::vec3(0.2f));	// it's a bit too big for our scene, so scale it down
		modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		drone.Draw(staticShader);

		//-----------------------------------------------------------------------------------------------------
		// PF - Rocket
		//-----------------------------------------------------------------------------------------------------
		//Scale global Rocket
		float scaleRocket = 0.3f;
		glm::vec3 vSRocket = glm::vec3(scaleRocket);

		//BaseRocket
		glm::mat4 modelBase = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 0.0f, 55.0f));
		modelBase = glm::rotate(modelBase, glm::radians(turnBase), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", glm::scale(modelBase, vSRocket));
		baseRocket.Draw(staticShader);

		//Support 1
		glm::mat4 modelSupport1 = glm::translate(modelBase, glm::vec3(7.4f, 2.6f, -10.9f) * scaleRocket);
		modelSupport1 = glm::rotate(modelSupport1, glm::radians(-movS1), glm::vec3(1.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", glm::scale(modelSupport1, vSRocket));
		support1.Draw(staticShader);

		//Support 2
		glm::mat4 modelSupport2 = glm::translate(modelBase, glm::vec3(-13.0f, 2.7f, -0.9f) * scaleRocket);
		modelSupport2 = glm::rotate(modelSupport2, glm::radians(movS2), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", glm::scale(modelSupport2, vSRocket));
		support2.Draw(staticShader);

		//Support 3
		glm::mat4 modelSupport3 = glm::translate(modelBase, glm::vec3(6.3f, 2.8f, 11.0f) * scaleRocket);
		modelSupport3 = glm::rotate(modelSupport3, glm::radians(movS3), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", glm::scale(modelSupport3, vSRocket));
		support3.Draw(staticShader);

		//Rocket
		glm::mat4 modelRocket = glm::translate(modelBase, glm::vec3(movRocket_x + 2.5f, movRocket_y + 29.5f, movRocket_z) * scaleRocket);
		modelRocket = glm::rotate(modelRocket, glm::radians(turnRocket), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", glm::scale(modelRocket, vSRocket));
		rocket.Draw(staticShader);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	//skybox.Terminate();
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);

	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.x--;

	//Animación wey camminando C1_walking
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
		animacion ^= true;
		estadoC1_walking = 1;
	}

	//Animación wey camminando C1_walking RESET
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		animacion ^= true;
		estadoC1_walking = 0;
		movC1_z = 0.0f;
	}

	//Car animation
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		animacion ^= true;

	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}

	//-----------------------------------------------------------------------------------------------------
	// PF - Rocket
	//-----------------------------------------------------------------------------------------------------
	if (key == GLFW_KEY_O && action == GLFW_PRESS) {
		aniRocket ^= true;
		if (stateRocket == 0) {
			stateRocket = 1;
		}
	}

	if (key == GLFW_KEY_F10 && action == GLFW_PRESS) {
		aniRocket = false;
		stateRocket = 0;
		turnRocket = 0.0f;
		turnBase = 0.0f;
		movRocket_x = 0.0f;
		movRocket_y = 0.0f;
		movRocket_z = 0.0f;
		movS1 = 0.0f;
		movS2 = 0.0f;
		movS3 = 0.0f;
	}


}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	
	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}
