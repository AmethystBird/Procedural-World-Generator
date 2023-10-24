#pragma once
#include <cstdlib>
#include <iostream>

#include "GL/glew.h"
#include "tests/tests.h"


void init(void);

int runTests(std::string value);

void loadTexture(GLuint& texture, std::string texturepath);

//Called on mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos);