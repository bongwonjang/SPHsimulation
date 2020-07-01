#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <vector>
#include <ctime>
#include <cstdlib>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define PI 3.141592

class ParticleSystem
{
public:
	ParticleSystem(float sysWidth, float sysHeight);
	~ParticleSystem();

	//////////////////////////////////////////////
	// System Parameter
	//////////////////////////////////////////////
	float width; float height;
	float halfWidth; float halfHeight;
	int sqrtNumberOfParticles;
	int numberOfParticles;

	unsigned int VAOID;
	unsigned int VBOID;
	unsigned int SHADERID;

	void initParticleSystemBuffer();

	//////////////////////////////////////////////
	// Particle's Informations
	//////////////////////////////////////////////
	glm::vec3 * force;
	glm::vec3 * accel;
	glm::vec3 * velocity;

	std::vector<glm::vec3> vertices; //position
	float particleRadius;
	float dt;
	glm::vec3 gravity;
	
	//////////////////////////////////////////////
	// SPH's Informations
	//////////////////////////////////////////////
	float * rho; //rho 밀도
	float * p; //pressure 유체압력

	float restDensity;
	float gasConstant;
	float H;
	float HSQ; // = H * H;
	float mass;	//changable
	float viscosity;
	
	//Smoothing Kernel Functions. Very Very Important. You can optimize them.
	float POLY6;		// = 315.0f / (65.0f * PI * pow(H, 9.0f));
	float SPIKY_GRAD;	// = -45.0f / (PI * pow(H, 6.0f));
	float VISC_LAP;		// = 45.0f / (PI * pow(H, 6.0f));

	//////////////////////////////////////////////
	// Hash's Informations
	//////////////////////////////////////////////
	float leftBoundary = 0;
	float rightBoundary = 40;
	float topBoundary = 60;
	float bottomBoundary = 0;
	float frontBoundary = 0;
	float backBoundary = 10;

	float gridSize = 1.0f;
	float cellSizeX = (rightBoundary - leftBoundary);
	float cellSizeY = (topBoundary - bottomBoundary);
	float cellSizeZ = (backBoundary - frontBoundary);
	std::vector<unsigned int> HashTable[40][60][10]; // (50 - 0) / gridSize

	void initParticleInformations();
	void storeParticleToHashTable();

	//////////////////////////////////////////////
	// Particle's Informations
	//////////////////////////////////////////////
	void update();
	void movement();
	
	//For SPH Simulation
	void computeDensity();
	void computeForce();
	void wallCollision();
};

ParticleSystem::ParticleSystem(float sysWidth, float sysHeight)
{
	width = sysWidth;		halfWidth = sysWidth / 2;
	height = sysHeight;		halfHeight = sysHeight / 2;

	numberOfParticles = 8000;

	force		= new glm::vec3[numberOfParticles]();
	accel		= new glm::vec3[numberOfParticles](); 
	velocity	= new glm::vec3[numberOfParticles]();

	rho			= new float[numberOfParticles](); //rho 밀도
	p			= new float [numberOfParticles](); //pressure 압력

	restDensity = 40.0f;
	gasConstant = 30.0f;
	H = 1.0f;
	HSQ = H * H;
	mass = 1.0f;
	viscosity = 4.0f;

	POLY6 = 4.0f / (PI * pow(H, 8.0f));
	SPIKY_GRAD = -30.0f / (PI * pow(H, 8.0f));
	VISC_LAP = 40.0f / (PI * pow(H, 4.0f));

	dt = 0.025; //파티클 개수에 따라 조정가능!
	gravity = glm::vec3(0.0f, -9.8f, 0.0f);

	initParticleInformations();
}

ParticleSystem::~ParticleSystem()
{
	vertices.clear();
	delete[] force;
	delete[] accel;
	delete[] velocity;
	delete[] rho;
	delete[] p;
}

void ParticleSystem::initParticleSystemBuffer()
{
	glGenVertexArrays(1, &VAOID);
	glBindVertexArray(VAOID);
	
	unsigned int memSize = sizeof(float) * 3 * numberOfParticles;

	glGenBuffers(1, &VBOID);
	glBindBuffer(GL_ARRAY_BUFFER, VBOID);
	glBufferData(GL_ARRAY_BUFFER, memSize, vertices.data(), GL_DYNAMIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ParticleSystem::initParticleInformations()
{
	for (int i = 0; i < numberOfParticles; i++)
	{
		//Initialize force
		force[i].x = force[i].y = force[i].z = 0;

		//Initialize accel
		accel[i].x = accel[i].y = accel[i].z = 0;

		//Initialize velocity
		velocity[i].x = velocity[i].z = 0;	
		velocity[i].y = -10.0f;
	}

	//Initialize Particle positions
	srand(time(0));

	for (float i = 0; i < 10; i++)
	{
		for (float j = 0; j < 40; j++)
		{
			for (float k = 0; k < 10; k++)
			{
				float jitter = float(rand()) / float(RAND_MAX);

				vertices.push_back(glm::vec3(rightBoundary * 0.01 + (i * H + jitter / 8.0f),
					1.0f + j * H,
					frontBoundary * 0.01 + (k * H + jitter / 10.0f)));
				
				vertices.push_back(glm::vec3(rightBoundary * 0.99 - (i * H + jitter / 8.0f),
					1.0f + j * H,
					frontBoundary * 0.01 + (k * H + jitter / 10.0f)));
			}
		}
	}

	storeParticleToHashTable();
}
void ParticleSystem::storeParticleToHashTable()
{
	for (int i = 0; i < cellSizeX; i++)
		for (int j = 0; j < cellSizeY; j++)
			for (int k = 0; k < cellSizeZ; k++)
			HashTable[i][j][k].clear();

	for (int i = 0; i < numberOfParticles; i++)
	{
		int x = int(vertices[i].x);
		int y = int(vertices[i].y);
		int z = int(vertices[i].z);

		if (x < 0)
			x = 0;
		if (x > cellSizeX - 1)
			x = int(cellSizeX - 1);
		if (y < 0)
			y = 0;
		if (y > cellSizeY - 1)
			y = int(cellSizeY - 1);
		if (z < 0)
			z = 0;
		if (z > cellSizeZ - 1)
			z = int(cellSizeZ - 1);

		HashTable[x][y][z].push_back(i);
	}
}


void ParticleSystem::update()
{
	//////////////////////////////////////////////
	// Calculate Force, Accel, Velocity
	//////////////////////////////////////////////
	computeDensity();
	computeForce();
	movement();
	wallCollision();
	storeParticleToHashTable();

	glBindBuffer(GL_ARRAY_BUFFER, VBOID);
	void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(ptr, vertices.data(), sizeof(glm::vec3) * numberOfParticles);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}
void ParticleSystem::computeDensity()
{
	std::vector<unsigned int> res;

	for (int index = 0; index < numberOfParticles; index++)
	{
		int x = int(vertices[index].x);
		int y = int(vertices[index].y);
		int z = int(vertices[index].z);
		if (x < 0)
			x = 0;
		if (x > cellSizeX - 1)
			x = int(cellSizeX - 1);
		if (y < 0)
			y = 0;
		if (y > cellSizeY - 1)
			y = int(cellSizeY - 1);
		if (z < 0)
			z = 0;
		if (z > cellSizeZ - 1)
			z = int(cellSizeZ - 1);

		rho[index] = 0.0f;
		
		for (int i = x - 1; i <= x + 1; i++)
		{
			for (int j = y - 1; j <= y + 1; j++)
			{
				for (int k = z - 1; k <= z + 1; k++)
				{
					if (i < 0 || i >(cellSizeX - 1) || j < 0 || j >(cellSizeY - 1) || k < 0 || k > (cellSizeZ - 1))
						continue;

					for (int t = 0; t < HashTable[i][j][k].size(); t++)
						res.push_back(HashTable[i][j][k][t]);
				}
			}
		}

		for (int res_index = 0; res_index < res.size(); res_index++)
		{
			int particleIndex = res[res_index];
			glm::vec3 ij = vertices[particleIndex] - vertices[index];
			float r2 = glm::dot(ij, ij);

			if (r2 < HSQ)
			{
				rho[index] += (mass * POLY6 * pow(HSQ - r2, 3.0f));
			}

		}
		p[index] = gasConstant * (rho[index] - restDensity);
		res.clear();
	}
}
void ParticleSystem::computeForce()
{
	std::vector<unsigned int> res;

	for (int index = 0; index < numberOfParticles; index++)
	{
		int x = int(vertices[index].x);
		int y = int(vertices[index].y);
		int z = int(vertices[index].z);
		if (x < 0)
			x = 0;
		if (x > cellSizeX - 1)
			x = int(cellSizeX - 1);
		if (y < 0)
			y = 0;
		if (y > cellSizeY - 1)
			y = int(cellSizeY - 1);
		if (z < 0)
			z = 0;
		if (z > cellSizeZ - 1)
			z = int(cellSizeZ - 1);

		glm::vec3 fpress(0.0f);
		glm::vec3 fvisc(0.0f);
		
		for (int i = x - 1; i <= x + 1; i++)
		{
			for (int j = y - 1; j <= y + 1; j++)
			{
				for (int k = z - 1; k <= z + 1; k++)
				{
					if (i < 0 || i >(cellSizeX - 1) || j < 0 || j >(cellSizeY - 1) || k < 0 || k >(cellSizeZ - 1))
						continue;

					for (int t = 0; t < HashTable[i][j][k].size(); t++)
						res.push_back(HashTable[i][j][k][t]);
				}
			}
		}


		for (int res_index = 0; res_index < res.size(); res_index++)
		{
			int res_particleIndex = res[res_index];
			if (index == res_particleIndex)
				continue;

			glm::vec3 ij = vertices[res_particleIndex] - vertices[index];
			float r1 = glm::length(ij);

			if (r1 < H)
			{
				fpress += -glm::normalize(ij)*mass*(p[index] + p[res_particleIndex])
					/ (2.0f * rho[res_particleIndex]) * pow(H - (r1 * r1), 2.f) * SPIKY_GRAD;

				fvisc += viscosity * mass * (velocity[res_particleIndex] - velocity[index])
					/ rho[res_particleIndex] * (H - r1) * VISC_LAP;
			}
		}
		glm::vec3 fgrav = gravity * rho[index];
		force[index] = fpress + fvisc + fgrav;
		res.clear();
	}
}
void ParticleSystem::movement()
{
	for (int i = 0; i < numberOfParticles; i++)
	{
		//////////////////////////////////////////////
		// This Euler calcuation
		// accel[i] = force[i] / rho[i];
		// velocity[i] += accel[i] * dt;
		// vertices[i] += velocity[i] * dt;
		// 
		// But I'm gonna use RK4 calculation to optimize simulation
		//////////////////////////////////////////////
		
		glm::vec3 p1 = vertices[i];
		glm::vec3 v1 = velocity[i];
		glm::vec3 a1 = accel[i] = force[i] / rho[i];

		glm::vec3 p2 = p1 + v1 * dt / 2.0f;
		glm::vec3 v2 = v1 + a1 * dt / 2.0f;
		glm::vec3 a2 = (v2 - v1) / dt;

		glm::vec3 p3 = p1 + v2 * dt / 2.0f;
		glm::vec3 v3 = v1 + a2 * dt / 2.0f;
		glm::vec3 a3 = (v3 - v2) / dt;

		glm::vec3 p4 = p1 + v3 * dt;
		glm::vec3 v4 = v1 + a3 * dt;
		glm::vec3 a4 = (v4 - v3) / dt;

		velocity[i] += (a1 + 2.0f * a2 + 2.0f * a3 + a4) * dt / 6.0f;
		vertices[i] += (v1 + 2.0f * v2 + 2.0f * v3 + v4) * dt / 6.0f;
	}
}
void ParticleSystem::wallCollision()
{
	for (int i = 0; i < numberOfParticles; i++)
	{
		if (vertices[i].x >= rightBoundary)
		{
			vertices[i].x = rightBoundary - 0.01;
			velocity[i].x *= -0.7;
		}
		if (vertices[i].x  <= leftBoundary)
		{
			vertices[i].x = leftBoundary + 0.01;
			velocity[i].x *= -0.7;
		}
		if (vertices[i].y >= topBoundary)
		{
			vertices[i].y = topBoundary - 0.01;
			velocity[i].y *= -0.7;
		}
		if (vertices[i].y <= bottomBoundary)
		{
			vertices[i].y = bottomBoundary + 0.01;
			velocity[i].y *= -0.7;
		}
		if (vertices[i].z >= backBoundary)
		{
			vertices[i].z = backBoundary - 0.01;
			velocity[i].z *= -0.7;
		}
		if (vertices[i].z <= frontBoundary)
		{
			vertices[i].z = frontBoundary + 0.01;
			velocity[i].z *= -0.7;
		}

	}
}