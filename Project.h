#pragma once
#include <cstdlib>
#include <iostream>

#include "GL/glew.h"
#include "tests/tests.h"


void init(void);

int runTests(std::string value);

void loadTexture(GLuint& texture, std::string texturepath);

//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);