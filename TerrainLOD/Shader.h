#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <glad/glad.h>

class Shader
{
public:
	Shader() {}

	void setShader(const GLchar *path1, const GLchar *path2)
	{
		std::string myVs, myFrag;
		std::ifstream vs_file;
		std::ifstream frag_file;
		vs_file.exceptions(std::ifstream::badbit);
		frag_file.exceptions(std::ifstream::badbit);
		vs_file.open(path1);
		frag_file.open(path2);
		std::stringstream vsstream, fragstream;
		vsstream << vs_file.rdbuf();
		fragstream << frag_file.rdbuf();
		vs_file.close();
		frag_file.close();

		myVs = vsstream.str();
		myFrag = fragstream.str();

		GLuint vs;
		GLuint frag;
		GLchar error_log[512];
		GLint compiled;
		const GLchar *vs_code = myVs.c_str();
		const GLchar *frag_code = myFrag.c_str();

		vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vs_code, NULL);
		glCompileShader(vs);
		glGetShaderiv(vs, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			glGetShaderInfoLog(vs, 512, NULL, error_log);
			std::cout << "SHADER::VERTEX::COMPILATION_FAILED\n" << error_log << std::endl;
		}
		frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag, 1, &frag_code, NULL);
		glCompileShader(frag);
		glGetShaderiv(frag, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			glGetShaderInfoLog(frag, 512, NULL, error_log);
			std::cout << "SHADER::FRAGMENT::COMPILATION_FAILED\n" << error_log << std::endl;
		}
		this->shader_index = glCreateProgram();
		glAttachShader(this->shader_index, vs);
		glAttachShader(this->shader_index, frag);
		glLinkProgram(this->shader_index);
		glGetProgramiv(this->shader_index, GL_LINK_STATUS, &compiled);
		if (!compiled)
		{
			glGetProgramInfoLog(this->shader_index, 512, NULL, error_log);
			std::cout << "SHADER::PROGRAM::LINKING_FAILED\n" << error_log << std::endl;
		}
		glDeleteShader(vs);
		glDeleteShader(frag);
	}
	void Use()
	{
		glUseProgram(this->shader_index);
	}
	GLuint get_shaderindex()
	{
		return shader_index;
	}
private:
	GLuint shader_index;
};
#endif

