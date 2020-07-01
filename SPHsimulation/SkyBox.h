#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "TextureLoader.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <stdio.h>

class SkyBox
{
public:
	std::vector<std::string> faces;

	unsigned int textureID;

	int width, height, nrChannels; //for texture

	GLuint VAOID;
	GLuint VBOID;

	SkyBox();
	~SkyBox();
	void LoadSkyBoxTexture(std::vector<std::string> fs);
	void generateVBOVAO();
	GLuint getVBO()
	{
		return VBOID;
	}
	unsigned int getTextureID()
	{
		return textureID;
	}

	float skyboxVertices[108] = {
		// skybox vertices positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

};
SkyBox::SkyBox()
{
	/*
	Push Back 6 JPG files for skybox mapping
	*/
	faces.push_back("../texture/SkyBox/right.jpg");
	faces.push_back("../texture/SkyBox/left.jpg");
	faces.push_back("../texture/SkyBox/top.jpg");
	faces.push_back("../texture/SkyBox/bottom.jpg");
	faces.push_back("../texture/SkyBox/back.jpg");
	faces.push_back("../texture/SkyBox/front.jpg");

	//Load SkyBox Texture
	LoadSkyBoxTexture(faces);


	// Generate VBO VAO
	generateVBOVAO();
}
SkyBox::~SkyBox()
{
	faces.clear();
}
void SkyBox::LoadSkyBoxTexture(std::vector<std::string> fs)
{
	//Generate Texture ID
	glGenTextures(1, &textureID);

	//Bind Texture ID. We Use GL_TEXTURE_CUBE_MAP instead of GL_TEXTURE_2D
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int width, height, nrChannels;

	//We'll load 6 jpg images for skybox
	for (unsigned int i = 0; i < fs.size(); i++)
	{
		unsigned char *data = stbi_load(fs[i].c_str(), &width, &height, &nrChannels, 0);
		if (data != NULL)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			stbi_image_free(data);
			exit(1);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//To check textureid. if texture id is 0 or -1 then, there is a problem
	printf("skybox tex id : %d\n", textureID);
}
void SkyBox::generateVBOVAO()
{
	glGenVertexArrays(1, &VAOID);
	glBindVertexArray(VAOID);

	//Generate and Bind VBO.
	glGenBuffers(1, &VBOID);
	glBindBuffer(GL_ARRAY_BUFFER, VBOID);

	//Transfer Data
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//After finishing, UnBind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}