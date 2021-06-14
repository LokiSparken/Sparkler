#include "../imgui/imgui.h"
#include "../backends/imgui_impl_glfw.h"
#include "../backends/imgui_impl_opengl3.h"
#include "../misc/fonts/IconsFontAwesome5.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../includes/glm/glm.hpp"
#include "../includes/glm/gtc/matrix_transform.hpp"
#include "../includes/glm/gtc/type_ptr.hpp"

#include <map>
#include <cstdio>
#include <string>
#include <iostream>

#include "ui/console.h"
#include "const.h"
#include "shader.h"
#include "camera.h"
#include "model.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseClick_callback(GLFWwindow* window, int button, int action, int mods);
void moveCursor_callback(GLFWwindow* window, double xpos, double ypos);
void scrollCursor_callback(GLFWwindow* window, double x, double y);
void processInput(GLFWwindow* window);

Camera camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
unsigned char* image;
float blendValue = 0.6f;
float lastCursorX = WINDOW_WIDTH / 2.0f, lastCursorY = WINDOW_HEIGHT / 2.0f;
bool mouseClicked = false;
float lastFrame = 0.0f, deltaFrameTime = 0.0f;

int GlfwMode()
{
	glfwInit();

	const char* glsl_version = "#version 330";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Sparkler", NULL, NULL);
	// Create window with graphics context
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetMouseButtonCallback(window, mouseClick_callback);
	glfwSetCursorPosCallback(window, moveCursor_callback);
	glfwSetScrollCallback(window, scrollCursor_callback);

	// Initialize OpenGL loader
	gladLoadGL();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	ImGui::StyleColorsDark();
	io.Fonts->AddFontFromFileTTF("misc/fonts/Cousine-Regular.ttf", 16.0f);
	ImFontConfig config;
	config.MergeMode = true;
	config.PixelSnapH = true;
	config.GlyphMinAdvanceX = 13.0f;
	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	io.Fonts->AddFontFromFileTTF("misc/fonts/fontawesome-webfont.ttf", 13.0f, &config, icon_ranges);

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// State
	bool show_demo_window = true;
	bool show_console_window = true;
	ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

	//Shader shader = Shader(SHADER_PATH + "4. advanced_opengl/4.2 stencil_testing/object.vert", SHADER_PATH + "4. advanced_opengl/4.2 stencil_testing/object.frag");
	Shader shaderSkybox = Shader(SHADER_PATH + "4. advanced_opengl/4.6 cubemaps/skybox.vert", SHADER_PATH + "4. advanced_opengl/4.6 cubemaps/skybox.frag");
	//Shader shaderReflect = Shader(SHADER_PATH + "4. advanced_opengl/4.6 cubemaps/object_reflect.vert", SHADER_PATH + "4. advanced_opengl/4.6 cubemaps/object_reflect.frag");
	//Shader shaderSphereMapLoadTester = Shader(SHADER_PATH + "others/sphere_mapping/texture_load_tester.vert", SHADER_PATH + "others/sphere_mapping/texture_load_tester.frag");
	//Shader shaderSphereMap = Shader(SHADER_PATH + "others/sphere_mapping/sphere_map.vert", SHADER_PATH + "others/sphere_mapping/sphere_map.frag");
	//Shader shaderSphereMapWithSpotLight = Shader(SHADER_PATH + "others/sphere_mapping/sphere_map_with_spot_light.vert", SHADER_PATH + "others/sphere_mapping/sphere_map_with_spot_light.frag");
	Shader shaderSphereMapWithMultiLight = Shader(SHADER_PATH + "others/sphere_mapping/sphere_map_with_spot_light.vert", SHADER_PATH + "others/sphere_mapping/sphere_map_with_multi_light.frag");
	
	//std::string modelPath = "resources/objects/backpack/backpack.obj";
	//Model model(modelPath.c_str());

	std::vector<std::string> faces
	{
		"right",
		"left",
		"top",
		"bottom",
		"front",
		"back"
	};
	std::string cubemapPath = "resources/textures/skybox/5/";
	std::string fileExtension = ".jpg";
	for (int i = 0; i < 6; ++i)
	{
		faces[i] = cubemapPath + faces[i] + fileExtension;
		std::cout << "path: " << faces[i] << std::endl;
	}
	
	unsigned int cubemapTexture = loadCubemap(faces, false);

	// cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_normal_texcoord), &vertices_normal_texcoord, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	//std::string spheremapPath = "resources/textures/skybox/Stellarium3.jpg";
	//unsigned int spheremapTexture = loadTexture(spheremapPath.c_str(), false, GL_CLAMP_TO_EDGE);

	std::string diffuseMapPath("resources/textures/skybox/8k_sun.jpg");
	std::string specularMapPath("resources/textures/skybox/8k_sun.jpg");

	std::vector<std::string> sphereTexturesDiffuse =
	{
		"resources/textures/skybox/Stellarium3.jpg",
		"resources/textures/skybox/8k_earth_daymap.jpg",
		"resources/textures/skybox/ball1.jpg",
		"resources/textures/skybox/8k_jupiter.jpg",
		"resources/textures/skybox/2k_neptune.jpg",
		"resources/textures/skybox/8k_venus_surface.jpg",
		"resources/textures/skybox/8k_saturn.jpg",
		"resources/textures/skybox/4k_venus_atmosphere.jpg",
		"resources/textures/skybox/8k_mars.jpg"
	};

	std::vector<std::string> sphereTexturesSpecular =
	{
		"resources/textures/skybox/Stellarium3.jpg",
		"resources/textures/skybox/8k_earth_nightmap.jpg",
		"resources/textures/skybox/ball1.jpg",
		"resources/textures/skybox/8k_jupiter.jpg",
		"resources/textures/skybox/2k_neptune.jpg",
		"resources/textures/skybox/8k_venus_surface.jpg",
		"resources/textures/skybox/8k_saturn.jpg",
		"resources/textures/skybox/4k_venus_atmosphere.jpg",
		"resources/textures/skybox/8k_mars.jpg"
	};
	
	Sphere sphere(3.5f, 128, 128);
	sphere.loadSphereMap(diffuseMapPath, specularMapPath);
	sphere.initSphere();

	std::vector<Sphere> spheres;
	for (int i = 0; i < OBJECT_NUMBER - 1; ++i)
	{
		Sphere sphere(1.5f, 128, 128);
		sphere.loadSphereMap(sphereTexturesDiffuse[i], sphereTexturesSpecular[i]);
		sphere.initSphere();
		spheres.push_back(sphere);
	}

	glm::vec3 lightPosition = glm::vec3(1.2f, 1.0f, 2.0f);

	camera.setSpeed(2.5f);
	camera.setPosition(glm::vec3(0.0f, 0.0f, 20.0f));

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//if (show_demo_window)
		//	ImGui::ShowDemoWindow(&show_demo_window);

		consoleWindow(&show_console_window);

		processInput(window);

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		camera.setSpeed(cameraMoveSpeed);
		camera.setViewMatrix();
		
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		glm::mat4 viewMatrix = camera.getViewMatrix();
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.getFov()), 4.0f / 3.0f, 0.1f, 100.0f);

		// skybox cube
		glDepthFunc(GL_LEQUAL);
		shaderSkybox.use();
		shaderSkybox.setMat4("view", glm::mat4(glm::mat3(camera.getViewMatrix())));
		shaderSkybox.setMat4("projection", projectionMatrix);
		glBindVertexArray(cubeVAO);
		glBindTexture(GL_TEXTURE_2D, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// sphere map
		glDepthFunc(GL_LEQUAL);
		shaderSphereMapWithMultiLight.use();
		shaderSphereMapWithMultiLight.setMat4("view", viewMatrix);
		shaderSphereMapWithMultiLight.setMat4("projection", projectionMatrix);
		shaderSphereMapWithMultiLight.setVec3("viewPosition", camera.getPosition());
		shaderSphereMapWithMultiLight.setFloat("material.shininess", 4.0f);
		// directional light
		shaderSphereMapWithMultiLight.setVec3("directionalLight.direction", -0.2f, -1.0f, -0.3f);
		shaderSphereMapWithMultiLight.setVec3("directionalLight.ambient", 0.33f, 0.33f, 0.33f);
		shaderSphereMapWithMultiLight.setVec3("directionalLight.diffuse", 0.5f, 0.5f, 0.5f);
		shaderSphereMapWithMultiLight.setVec3("directionalLight.specular", 0.2f, 0.2f, 0.2f);
		// point light
		for (unsigned int i = 0; i < NR_POINT_LIGHTS; ++i)
		{
			std::string name = "pointLights[" + std::to_string(i) + std::string("].");
			//std::cout << name << std::endl;
			shaderSphereMapWithMultiLight.setVec3(name + "position", pointLightPositions[i]);
			shaderSphereMapWithMultiLight.setVec3(name + "ambient", 0.065f, 0.065f, 0.065f);
			shaderSphereMapWithMultiLight.setVec3(name + "diffuse", 0.15f, 0.15f, 0.15f);
			shaderSphereMapWithMultiLight.setVec3(name + "specular", 0.1f, 0.1f, 0.1f);
			shaderSphereMapWithMultiLight.setFloat(name + "constant", 1.0f);
			shaderSphereMapWithMultiLight.setFloat(name + "linear", 0.09f);
			shaderSphereMapWithMultiLight.setFloat(name + "quadratic", 0.032f);
		}
		//shaderSphereMapWithMultiLight.setVec3("pointLight.position", lightPosition);
		//shaderSphereMapWithMultiLight.setVec3("pointLight.ambient", 0.33f, 0.33f, 0.33f);
		//shaderSphereMapWithMultiLight.setVec3("pointLight.diffuse", 0.5f, 0.5f, 0.5f);
		//shaderSphereMapWithMultiLight.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
		//shaderSphereMapWithMultiLight.setFloat("pointLight.constant", 1.0f);
		//shaderSphereMapWithMultiLight.setFloat("pointLight.linear", 0.09f);
		//shaderSphereMapWithMultiLight.setFloat("pointLight.quadratic", 0.032f);
		// spot light
		shaderSphereMapWithMultiLight.setVec3("spotLight.ambient", 0.33f, 0.33f, 0.33f);
		shaderSphereMapWithMultiLight.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		shaderSphereMapWithMultiLight.setVec3("spotLight.specular", 0.01f, 0.01f, 0.01f);
		shaderSphereMapWithMultiLight.setVec3("spotLight.position", camera.getPosition());
		shaderSphereMapWithMultiLight.setVec3("spotLight.direction", camera.getFront());
		shaderSphereMapWithMultiLight.setFloat("spotLight.cutoff", glm::cos(glm::radians(12.5f)));
		shaderSphereMapWithMultiLight.setFloat("spotLight.outerCutoff", glm::cos(glm::radians(17.5f)));
		shaderSphereMapWithMultiLight.setFloat("spotLight.constant", 1.0f);
		shaderSphereMapWithMultiLight.setFloat("spotLight.linear", 0.09f);
		shaderSphereMapWithMultiLight.setFloat("spotLight.quadratic", 0.032f);

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::rotate(modelMatrix, glm::radians((float)glfwGetTime() * 6.15f), glm::vec3(0.0f, 1.0f, 0.0f));
		sphere.setModelMatrix(glm::rotate(modelMatrix, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		sphere.draw(shaderSphereMapWithMultiLight);

		for (int i = 0; i < OBJECT_NUMBER - 1; ++i)
		{
			glm::mat4 modelMatrix(1.0f);
			modelMatrix = glm::translate(modelMatrix, cubePositions[i+1]);
			modelMatrix = glm::rotate(modelMatrix, glm::radians((float)glfwGetTime() * 12.3f), glm::vec3(0.0f, 1.0f, 0.0f));
			//shaderSphereMapWithMultiLight.setMat4("model", modelMatrix);
			spheres[i].setModelMatrix(modelMatrix);
			
			for (int j = 0; j < NR_POINT_LIGHTS; ++j)
			{
				std::string name = "pointLights[" + std::to_string(j) + std::string("].");
				//std::cout << name << std::endl;
				shaderSphereMapWithMultiLight.setVec3(name + "position", pointLightPositions[j]);
				shaderSphereMapWithMultiLight.setVec3(name + "ambient", 0.065f, 0.065f, 0.065f);
				shaderSphereMapWithMultiLight.setVec3(name + "diffuse", 0.15f, 0.15f, 0.15f);
				shaderSphereMapWithMultiLight.setVec3(name + "specular", 0.1f, 0.1f, 0.1f);
				shaderSphereMapWithMultiLight.setFloat(name + "constant", 1.0f);
				shaderSphereMapWithMultiLight.setFloat(name + "linear", 0.09f);
				shaderSphereMapWithMultiLight.setFloat(name + "quadratic", 0.032f);

				spheres[i].draw(shaderSphereMapWithMultiLight);
			}
		}

		float currentTime = glfwGetTime();
		deltaFrameTime = currentTime - lastFrame;
		lastFrame = currentTime;

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

int main(int, char**)
{
	GlfwMode();

	return 0;
}

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void processInput(GLFWwindow* window)
{
	float scale = deltaFrameTime * 10.0f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.moveForward(scale);
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.moveBackward(scale);
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.moveLeft(scale);
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.moveRight(scale);
}

void mouseClick_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
			mouseClicked = true;
		else
			mouseClicked = false;
	}
}

void moveCursor_callback(GLFWwindow* window, double xpos, double ypos)
{
	float deltaX = xpos - lastCursorX, deltaY = ypos - lastCursorY;
	lastCursorX = xpos, lastCursorY = ypos;

	if (!mouseClicked) return;

	deltaX *= camera.getRotateSensitivity();
	deltaY *= camera.getRotateSensitivity();

	camera.addYaw(-deltaX);
	camera.addPitch(-deltaY);

	camera.moveCursor();
}

void scrollCursor_callback(GLFWwindow* window, double x, double y)
{
	camera.scrollCursor((float)y);
}
