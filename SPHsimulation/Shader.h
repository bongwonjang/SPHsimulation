#pragma once

#include <cstdio>
#include <string>
#include <fstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Shader
{
public:
	Shader() {};
	~Shader() { glDeleteProgram(programID); };
	
	void loadShader(const char * vertexShaderPath, const char * fragmentShaderPath, int mode);
	unsigned int createShader(const char * path, GLuint shaderMode);
	std::string readFile(const char * filePath);

	unsigned int programID;
	void use()
	{
		if (programID != 0)
			glUseProgram(programID);
		else
			printf("You Forgot to Load Shader First!");
	}

	void setBool(const char * name, bool value)
	{
		glUniform1i(glGetUniformLocation(programID, name), (int)value);
	}
	void setInt(const char * name, int value)
	{
		glUniform1i(glGetUniformLocation(programID, name), value);
	}
	void setFloat(const char * name, float value)
	{
		glUniform1i(glGetUniformLocation(programID, name), value);
	}
	void setVec2(const char * name, glm::vec2 value)
	{
		glUniform2fv(glGetUniformLocation(programID, name), 1, &value[0]);
	}
	void setVec2(const char * name, float x, float y)
	{
		glUniform2f(glGetUniformLocation(programID, name), x, y);
	}
	void setVec3(const char * name, glm::vec3 value)
	{
		glUniform3fv(glGetUniformLocation(programID, name), 1, &value[0]);
	}
	void setVec3(const char * name, float x, float y, float z)
	{
		glUniform3f(glGetUniformLocation(programID, name), x, y, z);
	}
	void setVec4(const char * name, glm::vec4 value)
	{
		glUniform4fv(glGetUniformLocation(programID, name), 1, &value[0]);
	}
	void setVec4(const char * name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(programID, name), x, y, z, w);
	}
	void setMat2(const char * name, glm::mat2 mat)
	{
		glUniformMatrix2fv(glGetUniformLocation(programID, name), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat3(const char * name, glm::mat3 mat)
	{
		glUniformMatrix3fv(glGetUniformLocation(programID, name), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat4(const char * name, glm::mat4 mat)
	{
		glUniformMatrix4fv(glGetUniformLocation(programID, name), 1, GL_FALSE, &mat[0][0]);
	}
};
void Shader::loadShader(const char * vertexShaderPath, const char * fragmentShaderPath, int mode)
{
	/*
		"../GLSL/VertexHDR.glsl";
		"../GLSL/FragmentHDR.glsl";
	*/
	if (vertexShaderPath == 0 || fragmentShaderPath == 0)
	{
		printf("Can't Load Shader\n");
		exit(0);
	}

	unsigned int vertShader = createShader(vertexShaderPath, GL_VERTEX_SHADER);
	unsigned int fragShader = createShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

	printf("vertexShader: %d    fragmentShader: %d\n", vertShader, fragShader);

	//if(mode == 0)
	programID = glCreateProgram();

	glAttachShader(programID, vertShader);
	glAttachShader(programID, fragShader);

	glLinkProgram(programID);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	printf("Shader Loaded\n");
}
unsigned int Shader::createShader(const char * path, GLuint shaderMode)
{
	if (path == 0)
	{
		printf("No Shader Path\n");
		return -1;
	}

	unsigned int shader = glCreateShader(shaderMode);

	std::string shaderStr = readFile(path);
	const char * shaderSrc = shaderStr.c_str();

	printf("Compiling Shader\n");
	glShaderSource(shader, 1, &shaderSrc, 0);
	glCompileShader(shader);

	return shader;
}
std::string Shader::readFile(const char * filePath)
{
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		printf("File does not exist\n");
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}