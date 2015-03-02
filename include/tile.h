#pragma once

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// My own class headers
#include <cube.h>

// ------- Tile class --------

#define MAXCUBES 200

class Tile{
public:
	Cube cubes[MAXCUBES];
	int num;
	int pivot;
	int form,trans;
	glm::ivec3 pos;
	
	GLfloat speed;
	GLfloat posTemp;

	Tile():speed(-0.5f),form(0),trans(0){
		this->num = 0;
		this->pivot = 0;
		this->pos = glm::ivec3(0,0,0);
	}

	Tile(int num,Cube cubes[],int form, glm::ivec3 pos, int pivot=2):speed(-0.5f),trans(0){
		this->num = num;
		for(int i=0;i<num;i++)
			this->cubes[i] = cubes[i];
		this->form = form;
		this->pivot = pivot;
		this->pos = pos;
		this->posTemp = this->pos.y;
	}

	Tile operator = (const Tile t){
		this->num = t.num;
		this->form = t.form;
		this->pivot = t.pivot;
		this->trans = t.trans;
		this->pos = t.pos;
		this->speed = t.speed;
		this->posTemp = t.posTemp;
		for(int i=0;i<this->num;i++)
			this->cubes[i] = t.cubes[i];

		return *this;
	}

	Cube& operator [] (int n){
		return this->cubes[n];
	}

	/*
	Tile():speed(glm::vec3(0.1f,0.0f,0.0f)),high_speed(glm::vec3(0.2f,0.0f,0.0f)){
		this->num = 4;
		this->pivot = 2;
		for(int i=0; i<this->num; i++)
	}
	*/

	glm::ivec3 GetPos(int n){ return this->cubes[n].pos + this->pos; }

	void Right(){ this->pos += glm::ivec3(1,0,0); }
	void Left(){ this->pos -= glm::ivec3(1,0,0); }
	void Down(){ this->pos -= glm::ivec3(0,1,0); }
	void Up(){ this->pos += glm::ivec3(0,1,0); }

	void UpdatePos(GLfloat deltaTime){
		this->posTemp += this->speed * deltaTime;
		this->pos.y = this->posTemp;
	}

	void RotateRight(int totTrans){
		//glm::mat4 rotate;
		//rotate = glm::rotate(rotate,glm::radians(90.0f),glm:vec3(0.0f,0.0f,1.0f));
		for(int i=0;i<this->num;i++){
			//this->cubes[i].pos = rotate * this->cubes[i].pos;
			glm::ivec3 &pos = this->cubes[i].pos;
			int temp = pos.y;
			pos.y = pos.x;
			pos.x = -temp;
		}
		trans = (trans + 1) % totTrans;
	}

	void RotateLeft(int totTrans){
		//glm::mat4 rotate;
		//rotate = glm::rotate(rotate,glm::radians(-90.0f),glm:vec3(0.0f,0.0f,1.0f));
		for(int i=0;i<this->num;i++){
			//this->cubes[i].pos = rotate * this->cubes[i].pos;
			glm::ivec3 &pos = this->cubes[i].pos;
			int temp = pos.y;
			pos.y = -pos.x;
			pos.x = temp;
		}
		trans = (trans - 1 + totTrans) % totTrans;
	}

	void Shuffle(){
		int tmp = cubes[num-1].type;
		for(int i=num-1; i>0; i--)
			cubes[i].type = cubes[i-1].type;
		cubes[0].type = tmp;
	}

	void SetPos(glm::ivec3 pos){
		this->pos = pos;
	}
};

struct TileLink{
	Tile tile;
	TileLink *next;
};

