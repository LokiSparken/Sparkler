#include "../imgui/imgui.h"
#include "../backends/imgui_impl_glfw.h"
#include "../backends/imgui_impl_opengl3.h"
#include "../misc/fonts/IconsFontAwesome5.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../includes/glm/glm.hpp"
#include "../includes/glm/gtc/matrix_transform.hpp"
#include "../includes/glm/gtc/type_ptr.hpp"

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
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

	//GlobalVariables 

	//Shader shader = Shader(SHADER_PATH + "4. advanced_opengl/4.1 depth_testing/depth_visual.vert", SHADER_PATH + "4. advanced_opengl/4.1 depth_testing/depth_visual.frag");
	Shader shader = Shader(SHADER_PATH + "4. advanced_opengl/4.2 stencil_testing/object.vert", SHADER_PATH + "4. advanced_opengl/4.2 stencil_testing/object.frag");
	Shader shaderOutlining = Shader(SHADER_PATH + "4. advanced_opengl/4.2 stencil_testing/outlining.vert", SHADER_PATH + "4. advanced_opengl/4.2 stencil_testing/outlining.frag");

	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_ALWAYS);
	glDepthFunc(GL_LESS);

	glEnable(GL_STENCIL_TEST);

	std::string modelPath = "resources/objects/backpack/backpack.obj";
	//std::string modelPath = "resources/objects/nanosuit/nanosuit.obj";
	//std::string modelPath = "resources/objects/planet/planet.obj";
	//std::string modelPath = "resources/objects/rock/rock.obj";
	Model model(modelPath.c_str());

	unsigned int cubeTexture = loadTexture("resources/textures/marble.jpg");
	unsigned int floorTexture = loadTexture("resources/textures/metal.png");

	// cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// plane VAO
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

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

		camera.setSpeed(cameraMoveSpeed);
		camera.setViewMatrix();

		shader.use();
		// view/projection transformations
		glm::mat4 projectionMatrix = glm::mat4(1.0f);
		projectionMatrix = glm::perspective(glm::radians(camera.getFov()), 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 viewMatrix = camera.getViewMatrix();
		shader.setMat4("view", viewMatrix);
		shader.setMat4("projection", projectionMatrix);
		glm::mat4 modelMatrix = glm::mat4(1.0f);

		// floor
		glStencilMask(0x00);
		shader.use();
		glBindVertexArray(planeVAO);
		glActiveTexture(GL_TEXTURE0);
		shader.setInt("textureDiffuse1", 0);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		shader.setMat4("model", modelMatrix);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		// cube
		glStencilFunc(GL_ALWAYS, 1, 0x00);
		shader.use();
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		shader.setMat4("model", glm::translate(modelMatrix, glm::vec3(-1.0f, 0.0f, -1.0f)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		shader.setMat4("model", glm::translate(modelMatrix, glm::vec3(2.0f, 0.0f, 0.0f)));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		// bag
		// 1. basic implementation
		//shader.setMat4("model", glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.25f)));
		//glStencilFunc(GL_ALWAYS, 1, 0xFF);
		//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		//glStencilMask(0xFF);
		//model.draw(shader);

		//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		//glStencilMask(0x00);
		//glDisable(GL_DEPTH_TEST);
		//shaderOutlining.use();
		//shaderOutlining.setMat4("model", glm::scale(modelMatrix, glm::vec3(0.26f, 0.26f, 0.26f)));
		//shaderOutlining.setMat4("view", viewMatrix);
		//shaderOutlining.setMat4("projection", projectionMatrix);
		//model.draw(shaderOutlining);
		//glStencilMask(0xFF);
		//glEnable(GL_DEPTH_TEST);

		// 2. implement in class : Model
		shaderOutlining.use();
		shaderOutlining.setMat4("view", viewMatrix);
		shaderOutlining.setMat4("projection", projectionMatrix);
		model.setModelMatrix(glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.25f)));
		model.draw(shader, true, shaderOutlining);

		float currentTime = glfwGetTime();
		deltaFrameTime = currentTime - lastFrame;
		lastFrame = currentTime;

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

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
