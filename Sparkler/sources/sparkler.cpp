#include "../imgui/imgui.h"
#include "../backends/imgui_impl_glfw.h"
#include "../backends/imgui_impl_opengl3.h"
#include "../misc/fonts/IconsFontAwesome5.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <iostream>

#include "../sources/ui/console.h"
#include "../sources/const.h"
#include "../sources/shader.h"
#include "../sources/camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../includes/stb_image.h"

#include "../includes/glm/glm.hpp"
#include "../includes/glm/gtc/matrix_transform.hpp"
#include "../includes/glm/gtc/type_ptr.hpp"

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
//bool firstEntry = true;
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

	Shader shader = Shader(SHADER_PATH + "light_test_cube.vert", SHADER_PATH + "light_test_cube.frag");
	Shader lightShader = Shader(SHADER_PATH + "light.vert", SHADER_PATH + "light.frag");

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int lightVAO;
	glGenVertexArrays(2, &lightVAO);
	glBindVertexArray(lightVAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw cube
		shader.use();
		unsigned int modelLocation_cube = glGetUniformLocation(shader.ID, "model");
		unsigned int viewLocation_cube = glGetUniformLocation(shader.ID, "view");
		unsigned int projectionLocation_cube = glGetUniformLocation(shader.ID, "projection");

		glm::mat4 model_cube = glm::mat4(1.0f);
		glUniformMatrix4fv(modelLocation_cube, 1, GL_FALSE, glm::value_ptr(model_cube));

		camera.setViewMatrix();
		glm::mat4 view = camera.getViewMatrix();
		glUniformMatrix4fv(viewLocation_cube, 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.getFov()), 4.0f/3.0f, 0.1f, 100.0f);
		glUniformMatrix4fv(projectionLocation_cube, 1, GL_FALSE, glm::value_ptr(projection));
		
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// draw light cube
		lightShader.use();
		unsigned int modelLocation_light = glGetUniformLocation(lightShader.ID, "model");
		unsigned int viewLocation_light = glGetUniformLocation(lightShader.ID, "view");
		unsigned int projectionLocation_light = glGetUniformLocation(lightShader.ID, "projection");
		glm::vec3 lightPosition = glm::vec3(1.2f, 1.0f, 2.0f);
		glm::mat4 model_light = glm::mat4(1.0f);
		model_light = glm::translate(model_light, lightPosition);
		model_light = glm::scale(model_light, glm::vec3(0.1f));
		glUniformMatrix4fv(modelLocation_light, 1, GL_FALSE, glm::value_ptr(model_light));
		glUniformMatrix4fv(viewLocation_light, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLocation_light, 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		float currentTime = glfwGetTime();
		deltaFrameTime = currentTime - lastFrame;
		lastFrame = currentTime;

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

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

