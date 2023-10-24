//GLEW
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"

//SHADERS
#include "LoadShaders.h"

//GLM
#include "glm/ext/vector_float3.hpp"
#include <glm/ext/matrix_transform.hpp> // GLM: translate, rotate
#include <glm/gtc/type_ptr.hpp> // GLM: access to the value_ptr

//STB
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//GENERAL
#include <iostream>
#include <vector>

//SPECIALISED
#include "Project.h"
#include "FastNoiseLite.h"
#include "FileReader.h"

using namespace std;
using namespace glm;

// to use this example you will need to download the header files for GLM put them into a folder which you will reference in
// properties -> VC++ Directories -> libraries

//VAO vertex attribute positions in correspondence to vertex attribute type
enum VAO_IDs { Triangles, Indices, Colours, Tex, NumVAOs = 2 };
//VAOs
GLuint  VAOs[NumVAOs];

//Buffer types
enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 };
//Buffer objects
GLuint  Buffers[NumBuffers];

enum Attrib_IDs { vPosition = 0, cPosition = 1, tPosition = 2 };
GLuint texture1;

//----------------------------------------------------------------------------
//
// init
//
#define BUFFER_OFFSET(a) ((void*)(a))

GLuint program;

bool drawWireframe = false;

//Window
#define SCREEN_SIZE_X 1280
#define SCREEN_SIZE_Y 720

#define RENDER_DISTANCE 128 //Render width of map
#define MAP_SIZE RENDER_DISTANCE * RENDER_DISTANCE //Size of map in x & z space

#define TOTAL_VERTICES MAP_SIZE

//Creation of map indices
const int squaresRow = RENDER_DISTANCE - 1;
const int squaresGrid = squaresRow * squaresRow;
const int trianglesPerSquare = 2;

const int trianglesRow = squaresRow * trianglesPerSquare;
const int trianglesGrid = trianglesRow * squaresRow;

//Transformations
//Relative position within world space
vec3 cameraPosition = vec3(0.0f, 0.0f, 3.0f);
//The direction of travel
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
//Up position within world space
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

//Scene perception construction; model & view
mat4 model;
mat4 projection;

//Camera sideways rotation
float cameraYaw = -90.0f;
//Camera vertical rotation
float cameraPitch = 0.0f;
//Determines if first entry of mouse into window
bool mouseFirstEnter = true;
//Positions of camera from given last frame
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
//Field of vision
float fov = 45.0f;

//Time
//Time change
float deltaTime = 0.0f;
//Last value of time change
float lastFrame = 0.0f;

void
init(void)
{
	//Load shaders
	ShaderInfo  shaders[] =
	{
		{ GL_VERTEX_SHADER, "media/mapShader.vert" },
		{ GL_FRAGMENT_SHADER, "media/mapShader.frag" },
		{ GL_NONE, NULL }
	};

	program = LoadShaders(shaders);
	glUseProgram(program);

	GLfloat mapVertices[MAP_SIZE][6];

	//Assigning perlin noise type for map
	FastNoiseLite mapHeightNoise;
	mapHeightNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin); //OG: NoiseType_OpenSimplex2
	mapHeightNoise.SetFrequency(0.05f);
	int newSeed = rand() % 100;
	mapHeightNoise.SetSeed(newSeed);

	//Gathering of noise data
	vector<float> mapHeightNoiseData(MAP_SIZE);
	int index = 0;

	//Generation of height map
	int i = 0;
	for (int y = 0; y < RENDER_DISTANCE; y++)
	{
		for (int x = 0; x < RENDER_DISTANCE; x++)
		{
			float noiseValue = mapHeightNoise.GetNoise((float)x, (float)y);

			mapVertices[i][1] = noiseValue; // / 100.f;
			i++;
		}
	}

	float drawingStartPosition = 1.0f; //0.5f for 16x16
	float columnShift = drawingStartPosition;
	float rowShift = drawingStartPosition;

	int j = 0;
	for (int i = 0; i < MAP_SIZE; i++)
	{
		//Generation of x & z vertices for horizontal plane
		mapVertices[i][0] = columnShift;

		mapVertices[i][2] = rowShift;

		//Determination of biomes based on height
		if (mapVertices[i][1] >= (0.5f / 8.0f))
		{
			//snow
			mapVertices[i][3] = 1.0f;
			mapVertices[i][4] = 1.0f;
			mapVertices[i][5] = 1.0f;
		}
		else
		{
			//grass
			mapVertices[i][3] = 0.0f;
			mapVertices[i][4] = 1.0f;
			mapVertices[i][5] = 0.0f;
		}

		columnShift = columnShift + -0.0625f;
		j++;
		if (j == RENDER_DISTANCE)
		{
			j = 0;
			columnShift = drawingStartPosition;
			rowShift = rowShift + -0.0625f;
		}
	}

	GLuint mapIndices[trianglesGrid][3];

	int indexColumnShift = 0;
	int indexRowShift = 0;

	//Generation of map indices in the form of 1x1 right angle triangle squares
	j = 0;
	for (int i = 0; i < trianglesGrid - 1; i += 2)
	{
		//vertices
		mapIndices[i][0] = indexColumnShift + indexRowShift; //top left
		//mapIndices[i][1] = 1 + indexColumnShift + indexRowShift; //top right
		mapIndices[i][2] = RENDER_DISTANCE + indexColumnShift + indexRowShift; //bottom left
		mapIndices[i][1] = 1 + indexColumnShift + indexRowShift; //top right

		mapIndices[i + 1][0] = 1 + indexColumnShift + indexRowShift; //top right
		mapIndices[i + 1][2] = RENDER_DISTANCE + indexColumnShift + indexRowShift; //bottom left
		mapIndices[i + 1][1] = 1 + RENDER_DISTANCE + indexColumnShift + indexRowShift; //bottom right

		indexColumnShift = indexColumnShift + 1;

		j++;
		if (j == squaresRow)
		{
			j = 0;
			indexColumnShift = 0;
			indexRowShift = indexRowShift + RENDER_DISTANCE;
		}
	}

	//Sets index of VAO
	glGenVertexArrays(NumVAOs, VAOs);
	//Binds VAO to a buffer
	glBindVertexArray(VAOs[0]);

	//Sets indexes of all required buffer objects
	glGenBuffers(NumBuffers, Buffers);

	//Binds vertex object (map vertices) to array buffer
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[Triangles]);
	//Allocates buffer memory for the vertices of the 'Triangles' buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(mapVertices), mapVertices, GL_STATIC_DRAW);

	//Binding & allocation for map indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[Indices]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mapIndices), mapIndices, GL_STATIC_DRAW);

	//Allocation & indexing of vertex attribute memory for vertex shader
	//Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //positions
	glEnableVertexAttribArray(0); //Enabling of vertex attribute; takes location

	//Colours
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //colours
	glEnableVertexAttribArray(1);

	//Unbinding
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Model matrix
	model = mat4(1.0f);
	//Scaling to zoom in
	model = scale(model, vec3(2.0f, 2.0f, 2.0f));
	//Rotation to look up
	model = rotate(model, radians(40.0f), vec3(1.0f, 0.0f, 0.0f));
	//Movement to reposition
	model = translate(model, vec3(0.0f, 0.0f, -1.0f));

	//Projection matrix
	projection = perspective(radians(fov), (float)SCREEN_SIZE_X / (float)SCREEN_SIZE_Y, 0.1f, 100.0f);

	//MAY BE PROBLEMATIC
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (mouseFirstEnter)
	{
		lastX = xpos;
		lastY = ypos;
		mouseFirstEnter = false;
	}

	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.01f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	cameraYaw += xOffset;
	cameraPitch += yOffset; //may be necessary

	//Prevention of looking behind oneself - appears to be buggy
	/*if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	else if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}*/

	//Direction vector
	vec3 direction;
	direction.x = cos(radians(cameraYaw)) * cos(radians(cameraPitch));
	direction.y = sin(radians(cameraPitch));
	direction.z = sin(radians(cameraYaw)) * cos(radians(cameraPitch));
	cameraFront = normalize(direction);

	cout << cameraPitch;
}

void processInput(GLFWwindow* window)
{
	//Extent to which to move in one instance
	const float cameraMovementSpeed = 0.5f * deltaTime;

	//WASD controls
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) //Forward
	{
		cameraPosition += cameraMovementSpeed * cameraFront;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) //Backward
	{
		cameraPosition -= cameraMovementSpeed * cameraFront;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) //Right
	{
		cameraPosition += normalize(cross(cameraFront, cameraUp)) * cameraMovementSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) //Left
	{
		cameraPosition -= normalize(cross(cameraFront, cameraUp)) * cameraMovementSpeed;
	}
	//Closes window on 'exit' key press
	else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) //Direction
	{
		glfwSetWindowShouldClose(window, true);
	}
	//Switch between triangle & wireframe rendering modes
	else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		drawWireframe = !drawWireframe;
	}
}

//Load and create a texture; unused
void loadTexture(GLuint& texture, std::string texturepath)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	GLint width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load(texturepath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

void
display(void)
{
	static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	glClearBufferfv(GL_COLOR, 0, black);
	glClear(GL_COLOR_BUFFER_BIT); //Clears the colour buffer

	// bind textures on corresponding texture units
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	//Transformations
	mat4 view;
	view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp); //Sets the position of the viewer, the movement direction in relation to it & the world up direction

	// Adding all matrices up to create combined matrix
	mat4 mvp = projection * view * model;

	//adding the Uniform to the shader
	int mvpLoc = glGetUniformLocation(program, "mvp");
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, value_ptr(mvp));

	//modify position using mv & p
	glBindVertexArray(VAOs[0]); //Bind buffer object to render;

	if (drawWireframe == false)
	{
		glDrawElements(GL_TRIANGLES, TOTAL_VERTICES * 32, GL_UNSIGNED_INT, 0); //GL_TRIANGLES for default, GL_LINES for wireframe
		return;
	}
	glDrawElements(GL_LINES, TOTAL_VERTICES * 32, GL_UNSIGNED_INT, 0);
}

int runTests(std::string value)
{
	doctest::Context ctx;
	if (value.compare("adv") == 0)
	{
		ctx.addFilter("test-case-exclude", "STD");
	}
	else
	{
		ctx.addFilter("test-case-exclude", "ADV");
	}
	ctx.setOption("abort-after", 10);  // default - stop after 5 failed asserts
	ctx.setOption("no-breaks", true); // override - don't break in the debugger
	int res = ctx.run();              // run test cases unless with --no-run
	if (ctx.shouldExit())              // query flags (and --exit) rely on this
		return res;                   // propagate the result of the tests
	// your actual program execution goes here - only if we haven't exited
	return res; // + your_program_res
}

int
main(int argc, char** argv)
{
	//Initialisation of GLFW
	glfwInit();

	//Initialisation of 'GLFWwindow' object
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Procedural World Generator", NULL, NULL);

	//Sets cursor to automatically bind to window & hides cursor pointer
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Mouse capture mode

	//Binds OpenGL to window
	glfwMakeContextCurrent(window);
	//Initialisation of GLEW
	glewInit();

	init();

	//Sets the mouse_callback() function as the callback for the mouse movement event
	glfwSetCursorPosCallback(window, mouse_callback);

	//Render loop
	while (!glfwWindowShouldClose(window))
	{
		//Time
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Input
		processInput(window); //Takes user input

		//Rendering, transformations & drawing
		display();

		//Refreshing
		glfwSwapBuffers(window); //Swaps the colour buffer
		glfwPollEvents(); //Queries all GLFW events
	}

	glfwDestroyWindow(window);

	//Safely terminates GLFW
	glfwTerminate();
}