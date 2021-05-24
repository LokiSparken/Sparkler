#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>

#include "../includes/glm/glm.hpp"
#include "../includes/glm/gtc/matrix_transform.hpp"
#include "../includes/glm/gtc/type_ptr.hpp"

static float cameraPosition[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
static float cameraMoveSpeed = 0.2f;

//class GlobalVariables
//{
//public:
//	static GlobalVariables& getInstance()
//	{
//		static GlobalVariables instance;
//		return instance;
//	}
//
//	glm::vec3 getObjectColorLogl();
//	glm::vec3 getLightColorLogl();
//
//	glm::vec3 setObjectColorLogl(glm::vec3 newColor);
//	glm::vec3 setLightColorLogl(glm::vec3 newColor);
//private:
//	GlobalVariables()
//	{
//		cameraMoveSpeed = 0.02f;
//		objectColor_LOGL_ = glm::vec3(65.0, 105.0, 225.0) / 255.0f;
//		lightColor_LOGL_ = glm::vec3(1.0f, 1.0f, 1.0f);
//	}
//
//	GlobalVariables(GlobalVariables const&);
//	void operator=(GlobalVariables const&);
//public:
//	float cameraMoveSpeed;
//	glm::vec3 objectColor_LOGL_;
//	glm::vec3 lightColor_LOGL_;
//
//	GlobalVariables(GlobalVariables const&) = delete;
//	void operator=(GlobalVariables const&) = delete;
//};
//
//glm::vec3 GlobalVariables::getObjectColorLogl()
//{
//	return objectColor_LOGL_;
//}
//
//glm::vec3 GlobalVariables::getLightColorLogl()
//{
//	return lightColor_LOGL_;
//}
//
//glm::vec3 GlobalVariables::setObjectColorLogl(glm::vec3 newColor)
//{
//	objectColor_LOGL_ = newColor;
//}
//
//glm::vec3 GlobalVariables::setLightColorLogl(glm::vec3 newColor)
//{
//	lightColor_LOGL_ = newColor;
//}

#endif
