#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>

class Camera
{
private:
	glm::vec3 cameraPos;
	glm::vec3 fwd;
	glm::vec3 rht;
	glm::vec3 up;

	float cameraDistance = 60;
	float fov = 45;
	float maxFOV = fov;
	float diffX = 0; //mouse <-> camera moving yaw
	float diffY = 0; //mouse <-> camera moving pitch

	const float PI1 = 3.141592f;
	const float PI2 = 6.283184f;

public:
	Camera(float posX, float posY, float posZ);

	glm::vec3 getCameraPos();
	void setCameraPos(glm::vec3 newCameraPos);

	glm::vec3 getFWD();
	void setFWD(glm::vec3 newFWD);

	glm::vec3 getRHT();
	void setRHT(glm::vec3 newRHT);

	glm::vec3 getUP();
	void setUP(glm::vec3 newUP);

	void addFOV(float p);
	void subFOV(float p);
	float getFOV();
	void setFOV(float pf);
	void subYAW(float p);
	void subPITCH(float p);
	float getYAW();
	float getPITCH();

	/*
	Important : based on spehre coordinate system
	*/
	void updateCameraPos()
	{
		fwd.x = -sin(diffX) * cos(diffY);
		fwd.y = sin(diffY);
		fwd.z = -cos(diffX) * cos(diffY);

		rht.x = -cos(diffX);
		rht.y = 0;
		rht.z = sin(diffX);

		up = glm::cross(fwd, rht);
		glm::normalize(fwd);
		glm::normalize(rht);
		glm::normalize(up);

		cameraPos.x = fwd.x * -cameraDistance;
		cameraPos.y = fwd.y * -cameraDistance;
		cameraPos.z = fwd.z * -cameraDistance;

	}
	glm::mat4 getProjection(float width, float height)
	{
		return glm::perspective(glm::radians(fov), (float)(width / height), 0.01f, 1000.0f);
	}
	glm::mat4 getView()
	{
		return glm::lookAt(cameraPos, glm::vec3(0, 0, 0), up);
		//return glm::lookAt(cameraPos, fwd, up);
	}
	glm::mat4 getMVP(float width, float height, glm::mat4 matrix)
	{
		glm::mat4 Projection = glm::perspective(glm::radians(fov), (float)(width / height), 0.01f, 1000.0f);
		glm::mat4 View = glm::lookAt(cameraPos, glm::vec3(0, 0, 0), up);

		//We'll use transpose. OpenGL supports column based matrix.
		glm::mat4 Model = glm::transpose(matrix);

		//The order is from right to left.
		return Projection * View  * Model;
	}
};
Camera::Camera(float posX, float posY, float posZ)
{
	cameraPos.x = posX; cameraPos.y = posY; cameraPos.z = posZ;
	fwd.x = 0; fwd.y = 0; fwd.z = 1;
	rht.x = 1; rht.x = 0; rht.x = 0;
	up.x = 0; up.y = 1; up.z = 0;

	cameraDistance = sqrt(posX * posX + posY * posY + posZ * posZ);

}
glm::vec3 Camera::getCameraPos()
{
	return cameraPos;
}
void Camera::setCameraPos(glm::vec3 newCameraPos)
{
	cameraPos = newCameraPos;
}
glm::vec3 Camera::getFWD()
{
	return fwd;
}
void Camera::setFWD(glm::vec3 newFWD)
{
	fwd = newFWD;
}
glm::vec3 Camera::getRHT()
{
	return rht;
}
void Camera::setRHT(glm::vec3 newRHT)
{
	rht = newRHT;
}
glm::vec3 Camera::getUP()
{
	return up;
}
void Camera::setUP(glm::vec3 newUP)
{
	up = newUP;
}
void Camera::addFOV(float p)
{
	if (fov < maxFOV)
		fov += p;
	else //limitation of Field of View
		fov = maxFOV;
}
void Camera::subFOV(float p)
{
	if (fov > 1)
		fov -= p;
	else //limitation of Field of View
		fov = 1;
}
float Camera::getFOV()
{
	return fov;
}
void Camera::subYAW(float p)
{
	//I fogot YAW
	diffX -= p;
	if (abs(diffX) >= PI2)
		diffX = 0;
}
void Camera::subPITCH(float p)
{
	//I fogot PITCH
	diffY -= p;
	if (abs(diffY) >= PI2)
		diffY = 0;
}
float Camera::getYAW()
{
	return diffX;
}
float Camera::getPITCH()
{
	return diffY;
}
void Camera::setFOV(float pf)
{
	fov = pf;
}