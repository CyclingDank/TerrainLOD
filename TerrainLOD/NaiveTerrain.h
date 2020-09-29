#pragma once
#include <iostream>
#include <glad/glad.h>
#include "glm/ext.hpp"
#include "SOIL2/SOIL2.h"
#include <cstring>
#include <string>
#include <vector>

#include "Shader.h"

struct NaiveTerrainMesh
{
	glm::vec3 vertices;
};

/*
******This class is (was) my original implementation of terrain via height map. Each pixel on the heightmap corresponds to a vertex.***********
*/

class NaiveTerrain
{
public:
	NaiveTerrain()
	{
		std::string terrTex[num_texture] = { "resources/images/terrainmap.png", "resources/images/terrain.png","resources/images/image5.png" };
		shaderProgram.setShader("naive_terrain_mesh.vs", "naive_terrain_mesh.frag");
		initializeMap(terrTex);
		initializeMesh();
		setBuffer();
	}
	void initializeMap(std::string filePath[])
	{
		glGenTextures(3, texture);
		for (int i = 0; i < num_texture; i++)
		{
			std::string placehold = filePath[i];
			int nwidth, nheight;
			unsigned char *image = SOIL_load_image(placehold.c_str(), &nwidth, &nheight, 0, SOIL_LOAD_RGBA);
			if (image)
			{
				glBindTexture(GL_TEXTURE_2D, texture[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nwidth, nheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				std::cout << "Texture failed to load: " << filePath << std::endl;
				return;
			}
			this->m = nwidth;
			SOIL_free_image_data(image);
		}
	}
	void initializeMesh()
	{
		for (int y = 0; y < m; y++)
		{
			for (int x = 0; x < m; x++)
			{
				mesh.push_back({ glm::vec3(x, 0, y) });
				if (y < m - 1 && x < m - 1)
				{
					indices.push_back(y*m + x);
					indices.push_back(y*m + x + 1);
					indices.push_back((y + 1)*m + x + 1);
					indices.push_back(y*m + x);
					indices.push_back((y + 1)*m + x);
					indices.push_back((y + 1)*m + x + 1);
				}
			}
		}
	}
	void setBuffer()
	{
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);

		glBindVertexArray(this->VAO);

		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, this->mesh.size() * sizeof(NaiveTerrainMesh), &this->mesh[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(NaiveTerrainMesh), (GLvoid *)0);
	}
	void placeMesh(glm::vec3 playerCoords, glm::mat4 view, glm::mat4 projection, bool cond)
	{
		glm::vec3 origin = glm::vec3(-m, 0.0f, -m);
		shaderProgram.Use();
		if (cond)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glm::mat4 model;
		model = glm::translate(model, origin);
		model = glm::scale(model, glm::vec3(16.0f, 1.0f, 16.0f));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glUniform1i(glGetUniformLocation(shaderProgram.get_shaderindex(), "heightmap"), 0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glUniform1i(glGetUniformLocation(shaderProgram.get_shaderindex(), "forestTex"), 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glUniform1i(glGetUniformLocation(shaderProgram.get_shaderindex(), "cityTex"), 3);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.get_shaderindex(), "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.get_shaderindex(), "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.get_shaderindex(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform1i(glGetUniformLocation(shaderProgram.get_shaderindex(), "terrain_length"), (m - 1)*64);

		glUniform3f(glGetUniformLocation(shaderProgram.get_shaderindex(), "lightObj.direction"), -.5f, -1.0f, -0.5f);
		glUniform3f(glGetUniformLocation(shaderProgram.get_shaderindex(), "lightObj.ambient"), 0.3f, 0.3f, 0.5f);
		glUniform3f(glGetUniformLocation(shaderProgram.get_shaderindex(), "lightObj.diffuse"), 0.9f, 0.8f, 0.7f);


		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}
	void clearTerrain()
	{
		glDeleteBuffers(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
private:
	Shader shaderProgram;
	std::string file;
	GLuint VAO, VBO, EBO;

protected:
	int m;
	static const int num_texture = 3;

	GLuint texture[num_texture];
	std::vector<NaiveTerrainMesh>mesh;
	std::vector<unsigned int> indices;
};