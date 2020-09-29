#pragma once
#include <iostream>
#include <glad/glad.h>
#include "glm/ext.hpp"
#include "SOIL2/SOIL2.h"
#include <cstring>
#include <string>
#include <vector>

#include "Shader.h"

struct TerrainMesh
{
	glm::vec3 vertices;
};
class Terrain
{
public:
	Terrain()
	{
		std::string terrTex[num_texture] = { "resources/images/terrainmap.png", "resources/images/terrain.png","resources/images/image5.png" };
		shaderProgram.setShader("terrain_mesh.vs", "terrain_mesh.frag");
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
		glBufferData(GL_ARRAY_BUFFER, this->mesh.size() * sizeof(TerrainMesh), &this->mesh[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainMesh), (GLvoid *)0);

	}

	void placeMesh(glm::vec3 playerCoords, glm::mat4 view, glm::mat4 projection, bool cond)
	{
		int levels_visible;
		if (playerCoords.y <= 55.0f)
			levels_visible = 5;
		else if (playerCoords.y > 55.0f && playerCoords.y <= 120.0f)
			levels_visible = 4;
		else if (playerCoords.y > 120.0f && playerCoords.y <= 235.0f)
			levels_visible = 3;
		else if (playerCoords.y > 235.0f && playerCoords.y <= 500.0f)
			levels_visible = 2;
		else
			levels_visible = 1;
		int aPos = (m - 1)*pow(2, num_levels - 1) * 2;
		glm::vec3 origin = glm::vec3(-aPos, 0.0f, -aPos);
		for (int i = 0; i < levels_visible; i++)
		{
			float scale = (pow(2, num_levels - 1 - i));
			float step = (m - 1) *pow(2, num_levels - 1 - i);
			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 4; x++)
				{
					glm::vec3 temp = origin + glm::vec3(step * x, 0.0f, step * y);
					temp += playerCoords;
					temp.y = 0.0f;
					if (!(((x == 1 || x == 2) && y == 1) || ((x == 1 || x == 2) && y == 2)))
					{
						renderTile(scale, temp, view, projection, cond);
					}
					else if (i == levels_visible - 1 && (((x == 1 || x == 2) && y == 1) || ((x == 1 || x == 2) && y == 2)))
					{
						renderTile(scale, temp, view, projection, cond);
					}
				}
			}
			origin += glm::vec3(step, 0, step);
		}
	}
	void renderTile(float scale, glm::vec3 meshLoc, glm::mat4 view, glm::mat4 projection, bool cond)
	{
		shaderProgram.Use();
		if (cond)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glm::mat4 model;
		model = glm::translate(model, meshLoc);
		model = glm::scale(model, glm::vec3(scale, scale, scale));
		int tile_size = (m - 1)*pow(2, num_levels - 1) * 4;

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
		glUniform1i(glGetUniformLocation(shaderProgram.get_shaderindex(), "terrain_length"), tile_size);


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
	static const int m = 33;
	static const int num_levels = 5;
	static const int num_texture = 3;

	GLuint texture[num_texture];
	std::vector<TerrainMesh>mesh;
	std::vector<unsigned int> indices;


};