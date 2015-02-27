#pragma once

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ---- Cube class ------

class Cube{
public:
	glm::ivec3 pos;
	int type;

	Cube(glm::ivec3 p, int t = 0):pos(p),type(t){}
	Cube(int x,int y,int z){
		pos = glm::ivec3(x,y,z);
		type = 0;
	}
	Cube(){
		pos = glm::ivec3(0,0,0);
		type = 0;
	}

	Cube operator = (const Cube c){
		this->pos = c.pos;
		this->type = c.type;
		return *this;
	}
};