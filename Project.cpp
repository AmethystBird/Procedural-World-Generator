//////////////////////////////////////////////////////////////////////////////
//
//  Triangles.cpp
//
//////////////////////////////////////////////////////////////////////////////

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"
#include "LoadShaders.h"
#include <glm/glm.hpp> //includes GLM
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include <glm/ext/matrix_transform.hpp> // GLM: translate, rotate
#include <glm/ext/matrix_clip_space.hpp> // GLM: perspective and ortho 
#include <glm/gtc/type_ptr.hpp> // GLM: access to the value_ptr
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include<vector>

#include "Project.h"

#include "FastNoiseLite.h"

#include "FileReader.h"

using namespace std;

// to use this example you will need to download the header files for GLM put them into a folder which you will reference in
// properties -> VC++ Directories -> libraries

enum VAO_IDs { Triangles, Indices, Colours, Tex, NumVAOs = 2 };
enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 };
enum Attrib_IDs { vPosition = 0, cPosition = 1, tPosition = 2 };

GLuint  VAOs[NumVAOs];
GLuint  Buffers[NumBuffers];
GLuint texture1;

//const GLuint  NumVertices = 24; //OG: 36

//----------------------------------------------------------------------------
//
// init
//
#define BUFFER_OFFSET(a) ((void*)(a))

GLuint program;

bool drawWireframe = false;

//Screen dimensions
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

//Camera
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//Field of vision
glm::mat4 model;
glm::mat4 projection;

//Direction
float yaw = -90.0f;
float pitch = 0.0f;
bool mouseFirstEnter = true;
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
float fov = 45.0f;

//Time
float deltaTime = 0.0f;
float lastFrame = 0.0f;

/*struct Vertex {
	GLfloat position[3];
	GLfloat normal[3];
};

class Mesh {
public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices);
	void Draw();
private:
	unsigned int VAO, VBO, EBO;
	void SetupMesh();
};

class Model
{
public:
	Model(char* path)
	{
		LoadModel(path);
	}
	void Draw();
private:
	vector<Mesh> meshes;
	string directory;

	void LoadModel(string path);
};*/

void
init(void)
{
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
		//mapIndices[i + 1][1] = 1 + RENDER_DISTANCE + indexColumnShift + indexRowShift; //bottom right
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

	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[0]);

	glGenBuffers(NumBuffers, Buffers);

	//Assigning map vertices to buffer
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[Triangles]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mapVertices), mapVertices, GL_STATIC_DRAW);

	//Assigning map indices to buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[Indices]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mapIndices), mapIndices, GL_STATIC_DRAW);

	//Interpretation of vertices/indices in memory
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //positions
	glEnableVertexAttribArray(0); //Enabling of vertex attribute; takes location
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //colours
	glEnableVertexAttribArray(1);

	//Unbinding
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Creation of the model matrix
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
	model = glm::rotate(model, glm::radians(40.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //OG: radians(-40.0f), vec3(1.0f, 0.0f, 0.0f)
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));

	//Creation of the projection matrix
	projection = glm::perspective(glm::radians(fov), (float)SCREEN_SIZE_X / (float)SCREEN_SIZE_Y, 0.1f, 100.0f);

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

	yaw += xOffset;
	pitch += yOffset; //may be necessary

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
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);

	cout << pitch;
}

void processInput(GLFWwindow* window)
{
	const float cameraMovementSpeed = 0.5f * deltaTime;
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
		cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraMovementSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) //Left
	{
		cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraMovementSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) //Direction
	{
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) //Switch between triangle & wireframe rendering modes
	{
		drawWireframe = !drawWireframe;
	}
}

//Unused
void loadTexture(GLuint& texture, std::string texturepath)
{
	// load and create a texture 
// -------------------------

// texture 1
// ---------
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
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


//----------------------------------------------------------------------------
//
// display
//


void
display(void)
{
	static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	glClearBufferfv(GL_COLOR, 0, black);
	glClear(GL_COLOR_BUFFER_BIT);
	// bind textures on corresponding texture units
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	//camera position
	glm::mat4 view;
	view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

	// Adding all matrices up to create combined matrix
	glm::mat4 mvp = projection * view * model;

	//adding the Uniform to the shader
	int mvpLoc = glGetUniformLocation(program, "mvp");
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

	//modify position using mv & p
	glBindVertexArray(VAOs[0]);

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
//----------------------------------------------------------------------------
//
// main
//
int
main(int argc, char** argv)
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Procedural World Generator", NULL, NULL);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Mouse capture mode

	glfwMakeContextCurrent(window);
	glewInit();

	init();

	glfwSetCursorPosCallback(window, mouse_callback);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		display();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
}

/*Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices)
{
	this->vertices = vertices;
	this->indices = indices;

	SetupMesh();
}

void Mesh::Draw()
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::SetupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		&indices[0], GL_STATIC_DRAW);

	//Vertices
	glEnableVertexAttribArray(0); //Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1); //Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glBindVertexArray(0);
}

void Model::Draw()
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw();
	}
}

void Model::LoadModel(string path)
{
	//Import with my own File Reader
	FileReader OBJFileReader;
	OBJFileReader.openFile("pogg");
	
	if (OBJFileReader.currentFileName == "")
	{
		cout << "Error loading model with FileReader.";
	}

	vector<Vertex> vertices;
	vector<Vertex> indices;
	//vector<Vector> textures;

	bool hitEnd = false;

	GLfloat loadingVertex[3];
	for (int i = 0; hitEnd == false; i++)
	{
		vector<string> vertex;
		vertex = OBJFileReader.getNextElement("vertex");

		if (vertex.front() == "")
		{
			hitEnd = true;
		}
		else
		{
			string posX = vertex[0];
			GLfloat posXValue = stof(posX);
			string posY = vertex[1];
			GLfloat posYValue = stof(posY);
			string posZ = vertex[2];
			GLfloat posZValue = stof(posZ);

			GLfloat allPosValues[3] = {
				posXValue, posYValue, posZValue
			};

			Vertex vertexToSend;
			vertexToSend.position = allPosValues;

			vertices[i].position = allPosValues;
		}
	}

	bool hitEnd = false;

	for (int i = 0; hitEnd == false; i++)
	{

	}
}*/
