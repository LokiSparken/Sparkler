#ifndef CAMERA_H
#define CAMERA_H

#include "../includes/glm/glm.hpp"
#include "../includes/glm/gtc/matrix_transform.hpp"
#include "../includes/glm/gtc/type_ptr.hpp"

#include "const.h"

class Camera
{
private:
	float rotateSensitivity_;
	float yaw_, pitch_;
	float fov_;

	glm::vec3 position_;
	glm::vec3 center_, up_, right_, front_;
	glm::mat4 viewMatrix_;

	//void constructViewMatrix();
	void updateViewMatrix();

public:
	float moveSpeed;
	//static float moveSpeed;

	Camera(glm::vec3 position);
	float getFov();
	float getRotateSensitivity();
	glm::vec3 getPosition();
	glm::vec3 getFront();
	glm::mat4 getViewMatrix();
	
	void setFov(float fov);
	void setSpeed(float speed);
	void setPosition(glm::vec3 position);
	void setViewMatrix();
	void setViewMatrix(glm::mat4 view);
	void addYaw(float deltaYaw);
	void addPitch(float deltaPitch);
	
	void moveForward(float scale);
	void moveBackward(float scale);
	void moveLeft(float scale);
	void moveRight(float scale);

	void moveCursor();
	void scrollCursor(float offset);
};

//float Camera::moveSpeed = 0.2f;

//void Camera::constructViewMatrix()
//{
//	glm::vec3 stareAt = position_ - center_;
//	right_ = glm::cross(stareAt, up_);
//
//	viewMatrix_ = glm::mat4(1.0f);
//	viewMatrix_[0][0] = right_.x, viewMatrix_[0][1] = up_.x, viewMatrix_[0][2] = stareAt.x, viewMatrix_[0][3] = ;
//}

Camera::Camera(glm::vec3 position)
{
	moveSpeed = 0.2f;
	rotateSensitivity_ = 0.1f;
	yaw_ = -90.0f, pitch_ = 0.0f;
	fov_ = 45.0f;

	position_ = position;
	center_ = glm::vec3(0.0f, 0.0f, 0.0f);
	up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	front_ = glm::vec3(0.0f, 0.0f, -1.0f);
	right_ = glm::normalize(glm::cross(front_, up_));

	viewMatrix_ = glm::lookAt(position_, center_, up_);
	//constructViewMatrix();
}

void Camera::addYaw(float deltaYaw)
{
	yaw_ += deltaYaw;
}

void Camera::addPitch(float deltaPitch)
{
	pitch_ += deltaPitch;

	if (pitch_ > 89.9f)
		pitch_ = 89.9f;
	else if (pitch_ < -89.9f)
		pitch_ = -89.9f;
}

void Camera::updateViewMatrix()
{
	center_ = position_ + front_;
	viewMatrix_ = glm::lookAt(position_, center_, up_);
}

float Camera::getFov()
{
	return fov_;
}

float Camera::getRotateSensitivity()
{
	return rotateSensitivity_;
}

glm::vec3 Camera::getPosition()
{
	return position_;
}

glm::vec3 Camera::getFront()
{
	return front_;
}

glm::mat4 Camera::getViewMatrix()
{
	return viewMatrix_;
}

void Camera::setFov(float fov)
{
	fov_ = fov;
	if (fov_ < 0.1f)
		fov_ = 0.1f;
	else if (fov_ > 60.0f)
		fov_ = 60.0f;
}

void Camera::setSpeed(float speed)
{
	moveSpeed = speed;
}

void Camera::setPosition(glm::vec3 position)
{
	position_ = position;
}

void Camera::setViewMatrix()
{
	updateViewMatrix();
}

void Camera::setViewMatrix(glm::mat4 view)
{
	viewMatrix_ = view;
}

void Camera::moveForward(float scale)
{
	position_ += moveSpeed * front_ * scale;
	updateViewMatrix();
}

void Camera::moveBackward(float scale)
{
	position_ -= moveSpeed * front_ * scale;
	updateViewMatrix();
}

void Camera::moveRight(float scale)
{
	right_ = glm::cross(front_, up_);
	position_ += moveSpeed * right_ * scale;
	updateViewMatrix();
}

void Camera::moveLeft(float scale)
{
	right_ = glm::cross(front_, up_);
	position_ -= moveSpeed * right_ * scale;
	updateViewMatrix();
}

void Camera::moveCursor()
{
	glm::vec3 direction;
	//direction.x = glm::cos(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_));
	direction.x = glm::cos(glm::radians(yaw_));
	direction.y = glm::sin(glm::radians(pitch_));
	direction.z = glm::sin(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_));
	front_ = glm::normalize(direction);
	updateViewMatrix();
}

void Camera::scrollCursor(float offset)
{
	setFov(getFov() + offset);
}


#endif // CAMERA_H
