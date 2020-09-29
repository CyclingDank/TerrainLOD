#include "pch.h"
#include "Scene.h"


void Scene::drawScene(Camera camera, glm::mat4 view, glm::mat4 projection, bool cond)
{
	terrainObj.placeMesh(camera.getPosition(), view, projection, cond);
	//naiveterrainObj.placeMesh(camera.getPosition(), view, projection, cond);
}

void Scene::erase()
{
	terrainObj.clearTerrain();
	//naiveterrainObj.clearTerrain();
}