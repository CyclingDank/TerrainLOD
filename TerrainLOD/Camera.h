#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera(glm::vec3 position)
	{
		this->position = position;
		this->pitch = -89.0f;
		this->forward = glm::normalize(glm::vec3(cos(glm::radians(this->pitch)), sin(glm::radians(this->pitch)), 0.0f));
		this->up = glm::normalize(glm::cross(glm::normalize(glm::cross(this->forward, glm::vec3(0.0f, 1.0f, 0.0f))), this->forward));
	}

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(this->position, this->position + this->forward, this->up);
	}

	glm::mat4 getProjectionMatrix(int SCREEN_WIDTH, int SCREEN_HEIGHT)
	{
		return glm::perspective(45.0f, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 10000.0f);
	}

	glm::vec3 getPosition()
	{
		return this->position;
	}

	void setPosition(glm::vec3 newPos)
	{
		this->position += newPos;
	}

private:
	GLfloat pitch;
	glm::vec3 position;
	glm::vec3 up;
	glm::vec3 forward;
};