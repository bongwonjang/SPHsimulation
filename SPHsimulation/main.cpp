#define _CRT_SECURE_NO_WARNINGS

//////////////////////////////////////////////
// Basic Header files
//////////////////////////////////////////////
#include <cstdio>
#include <GL/glew.h>
#include <GL/freeglut.h>

//////////////////////////////////////////////
// Additional Header files
//////////////////////////////////////////////
#include "ParticleSystem.h"
#include "Shader.h"
#include "Camera.h"
#include "SkyBox.h"

//////////////////////////////////////////////
// Window Constants
//////////////////////////////////////////////
const int windowWidth = 512;
const int windowHeight = 512;

//////////////////////////////////////////////
// Frame Buffer
//////////////////////////////////////////////
unsigned int FBOID[3];
unsigned int TEXID[3];

Shader Depth_Shader;
Shader Quad_Shader;

// ping-pong-framebuffer for "blurring" by LearnOpenGL
unsigned int pingpongFBO[2];
unsigned int pingpongColorbuffers[2];
unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

Shader Blur_Shader;

//////////////////////////////////////////////
// GL Part
//////////////////////////////////////////////
void initGL(int argc, char ** argv);
void initFrameBuffer();
void registerCallBackFunction();
	void display();
	void timer(int value);
	void mouseClick(int mouseEvt, int state, int x, int y);
		bool leftMouseClicked = false;
		bool middleMouseClicked = false;
		int mousePos[2] = { 0, 0 }; // point x and point y
	void mouseMotion(int x, int y);
void renderQuad();

Camera camera(0, 0, 60.0f);
SkyBox * skybox;
Shader SkyBox_Shader;

//////////////////////////////////////////////
// Simulation System Function
//////////////////////////////////////////////
ParticleSystem * ps;

void initParticleSystem();
void clearParticleSystem();

//////////////////////////////////////////////
// main
//////////////////////////////////////////////
int main(int argc, char ** argv)
{
	initGL(argc, argv);
	registerCallBackFunction();
	initFrameBuffer();
	
	Depth_Shader.loadShader("../GLSL/VertexDepth.glsl", "../GLSL/FragmentDepth.glsl", -1);
	Quad_Shader.loadShader("../GLSL/VertexQuad.glsl", "../GLSL/FragmentQuad.glsl", -1);
	Blur_Shader.loadShader("../GLSL/VertexBlur.glsl", "../GLSL/FragmentBlur.glsl", -1);
	SkyBox_Shader.loadShader("../GLSL/VertexSky.glsl", "../GLSL/FragmentSky.glsl", -1);
	skybox = new SkyBox();

	initParticleSystem(); //Initialize Particle system

	glutMainLoop();

	clearParticleSystem();

	return 0;
}
//////////////////////////////////////////////
// GL Part
//////////////////////////////////////////////
void initGL(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("2015410004 ÀåºÀ¿ø");

	glewInit(); //To use GPU
}
void initFrameBuffer()
{
	for (int i = 0; i < 3; i++)
	{
		glGenFramebuffers(1, &FBOID[i]);
		glBindFramebuffer(GL_FRAMEBUFFER, FBOID[i]);

		glGenTextures(1, &TEXID[i]);
		glBindTexture(GL_TEXTURE_2D, TEXID[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TEXID[i], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			exit(1);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			exit(1);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}
void registerCallBackFunction()
{
	glutDisplayFunc(display);
	glutTimerFunc(5, timer, 1);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);
	//glut...etc
}
void display()
{

	//////////////////////////////////////////////
	// 1. Render Scene into floating point FrameBuffer
	//////////////////////////////////////////////
	glBindFramebuffer(GL_FRAMEBUFFER, FBOID[0]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_POINT_SPRITE_ARB);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);

	Depth_Shader.use();

	glm::mat4 Projection = camera.getProjection(windowWidth, windowHeight);
	glm::mat4 View = camera.getView();
	glm::mat4 Model = (glm::translate(glm::mat4(1.0f), glm::vec3(ps->rightBoundary / -2.0f, 30.0f / -2.0f, ps->backBoundary / -2.0f)));
	glm::vec3 ViewPos = camera.getCameraPos();

	Depth_Shader.setMat4("Projection", Projection);
	Depth_Shader.setMat4("View", View);
	Depth_Shader.setMat4("Model", Model);
	Depth_Shader.setVec3("ViewPos", ViewPos);

	glBindVertexArray(ps->VAOID);

	glBindBuffer(GL_ARRAY_BUFFER, ps->VBOID);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_POINTS, 0, ps->numberOfParticles);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glUseProgram(0);
	glBindVertexArray(0);

	glDisable(GL_POINT_SPRITE_ARB);
	glDisable(GL_PROGRAM_POINT_SIZE);
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//////////////////////////////////////////////
	// 2. Draw Smoothed Depth
	//////////////////////////////////////////////
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	bool horizontal = true, first_iteration = true;
	unsigned int amount = 7;
	Blur_Shader.use();
	Blur_Shader.setInt("image", 0);
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		Blur_Shader.setInt("horizontal", horizontal);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? TEXID[0] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
		renderQuad();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//////////////////////////////////////////////
	// 3. Draw Background skybox image
	//////////////////////////////////////////////
	glBindFramebuffer(GL_FRAMEBUFFER, FBOID[2]); //To Use TEXID[2]

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glBindVertexArray(skybox->VAOID);
	glBindBuffer(GL_ARRAY_BUFFER, skybox->VBOID);

	SkyBox_Shader.use();
	SkyBox_Shader.setMat4("Projection", Projection);
	SkyBox_Shader.setMat4("View", View);
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getTextureID());

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(0));

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDepthFunc(GL_LESS);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//////////////////////////////////////////////
	// 4. Draw Normal Smoothed
	//////////////////////////////////////////////
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	Quad_Shader.use();
	Quad_Shader.setInt("scene", 0);
	Quad_Shader.setInt("backscene", 1);
	Quad_Shader.setMat4("Projection", Projection);
	Quad_Shader.setMat4("View", View);
	Quad_Shader.setMat4("Model", Model);
	Quad_Shader.setVec3("ViewPos", ViewPos);
	Quad_Shader.setMat4("inv_ProjectionMatrix", glm::transpose(camera.getProjection(windowWidth, windowHeight)));

	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getTextureID());
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TEXID[2]);

	renderQuad();
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);

	glutSwapBuffers();
}
void timer(int value)
{
	ps->update();
	
	glutPostRedisplay();
	glutTimerFunc(1, timer, 1);
}
void mouseClick(int mouseEvt, int state, int x, int y)
{
	mousePos[0] = x;
	mousePos[1] = y;

	if (mouseEvt == GLUT_MIDDLE_BUTTON)
	{
		middleMouseClicked = (GLUT_DOWN == state);
		leftMouseClicked = false;
	}
}
void mouseMotion(int x, int y)
{
	/*
		Middle Mouse : Rotate Camera Based on Sphere Coordinate
	*/
	if (middleMouseClicked)
	{
		camera.subYAW((x - mousePos[0]) * 0.0001f);
		camera.subPITCH((y - mousePos[1]) * 0.0001f);

		camera.updateCameraPos();
	}

	glutPostRedisplay();
}
//////////////////////////////////////////////
// I'v got this renderQuad function from LearnOpenGL. Because It works well
//////////////////////////////////////////////
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
//////////////////////////////////////////////
// Simulation System Function
//////////////////////////////////////////////
void initParticleSystem()
{
	ps = new ParticleSystem(windowWidth, windowHeight);
	ps->initParticleSystemBuffer();
}
void clearParticleSystem()
{
	delete ps;
	delete skybox;
}