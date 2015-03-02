#pragma once

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Robot class 

class Robot{
public:
	glm::vec3 basePos;
	glm::vec3 baseScale;
	glm::vec3 lowArmScale,highArmScale;
	float lowArmAngle,highArmAngle;

	glm::vec3 lowArmPos,highArmPos,tipPos;
	glm::vec3 lowArmMid,highArmMid;

	void UpdatePos(){
		lowArmPos = basePos + glm::vec3(0,baseScale.y / 2,0);
		highArmPos = lowArmPos + glm::vec3(lowArmScale.y * glm::sin(glm::radians(lowArmAngle)), 
				lowArmScale.y * glm::cos(glm::radians(lowArmAngle)), 0);
		tipPos = highArmPos + glm::vec3(highArmScale.y * glm::sin(glm::radians(highArmAngle)), 
				lowArmScale.y * glm::cos(glm::radians(highArmAngle)), 0);
		//lowArmMid = (lowArmPos + highArmPos) / 2;
		//highArmMid = (highArmPos + tipPos) /2;
		lowArmMid = lowArmPos + glm::vec3(lowArmScale.y /2 * glm::sin(glm::radians(lowArmAngle)), 
				lowArmScale.y /2 * glm::cos(glm::radians(lowArmAngle)), 0);
		highArmMid = highArmPos + glm::vec3(highArmScale.y /2 * glm::sin(glm::radians(highArmAngle)), 
				lowArmScale.y /2 * glm::cos(glm::radians(highArmAngle)), 0);
	}

	Robot(){
		baseScale = glm::vec3(5,2,5);
		basePos = glm::vec3(0,0,0);
		lowArmScale = glm::vec3(1,15,1);
		highArmScale = glm::vec3(1,15,1);
		lowArmAngle = 15.0f;
		highArmAngle = 90.0f;
		UpdatePos();
	}

	Robot(float basex,float lowy,float highy){
		baseScale = glm::vec3(5,1,5);
		//basePos = glm::vec3(basex,baseScale.y / 2,0);
		basePos = glm::vec3(basex,0,0);
		lowArmScale = glm::vec3(1,lowy,1);
		highArmScale = glm::vec3(1,highy,1);
		lowArmAngle = 15.0f;
		highArmAngle = 90.0f;
		UpdatePos();
	}

	Robot operator = (Robot r){
		this->basePos = r.basePos;
		this->baseScale = r.baseScale;
		this->lowArmScale = r.lowArmScale;
		this->highArmScale = r.highArmScale;
		this->lowArmAngle = r.lowArmAngle;
		this->highArmAngle = r.highArmAngle;
		UpdatePos();

		return *this;
	}

	void Right(float deltaTime){
		float angle = deltaTime * 30.0f;

		lowArmAngle += angle;
		if(lowArmAngle < 0.0f) lowArmAngle = 0.0f;
		if(lowArmAngle > 90.0f) lowArmAngle = 90.0f;
		UpdatePos();
	}

	void Left(float deltaTime){
		float angle = deltaTime * 30.0f;

		lowArmAngle -= angle;
		if(lowArmAngle < 0.0f) lowArmAngle = 0.0f;
		if(lowArmAngle > 90.0f) lowArmAngle = 90.0f;
		UpdatePos();
	}

	void Up(float deltaTime){
		float angle = deltaTime * 30.0f;

		highArmAngle -= angle;
		if(highArmAngle < 0.0f) highArmAngle = 0.0f;
		if(highArmAngle > 180.0f) highArmAngle = 180.0f;
		UpdatePos();
	}

	void Down(float deltaTime){
		float angle = deltaTime * 30.0f;

		highArmAngle += angle;
		if(highArmAngle < 0.0f) highArmAngle = 0.0f;
		if(highArmAngle > 180.0f) highArmAngle = 180.0f;
		UpdatePos();
	}

};

