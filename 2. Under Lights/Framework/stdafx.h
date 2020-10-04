#pragma once
#include <iostream>
#include <math.h>
#include <algorithm>
#include<vector>
#include<unordered_map>
#include<fstream>
#include<string>
#include<chrono>

//#include "GL/glew.h"
//#include "GL/freeglut.h"
//#include "glm/glm.hpp"
//#include "glm/ext.hpp"
//#include "glm/gtc/matrix_transform.hpp"

//¼±¹Ì
#include "gl/glew.h"
#include "gl/freeglut.h"
#include "gl/glm/glm.hpp"
#include "gl/glm/ext.hpp"
#include "gl/glm/gtc/matrix_transform.hpp"
using namespace std;


struct BasicObjectDesc
{
	GLuint primitiveType = GL_TRIANGLES;
};



struct VertexData
{
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
};


const glm::vec4 BASIC_COLOR[5] = { {1,0,0, 1},{0,1,0, 1},{0,0,1, 1},{1,1,0, 1},{0,1,1,1} };

enum COLOR
{
	RED,
	GREEN,
	BLUE,
	YELLOW,
	CYAN
};