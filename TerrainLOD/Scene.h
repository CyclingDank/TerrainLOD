#pragma once
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include "glm/ext.hpp"
#include <glm/glm.hpp>
#include "Shader.h"
#include "Terrain.h"
#include "NaiveTerrain.h"
#include "Camera.h"

class Scene
{
public: 
	Scene(){}

	void drawScene(Camera camera, glm::mat4 view, glm::mat4 projection, bool cond);

	void erase();

private:
	Terrain terrainObj;
	NaiveTerrain naiveterrainObj;
};