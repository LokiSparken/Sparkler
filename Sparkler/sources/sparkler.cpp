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

#include "../sources/ui/console.h"
#include "../sources/const.h"
#include "../sources/shader.h"
#include "../sources/camera.h"
#include "../sources/texture.h"


/*============= ASSIMP TEST =============*/
#include <../includes/assimp/Importer.hpp>      // C++ importer interface
#include <../includes/assimp/scene.h>           // Output data structure
#include <../includes/assimp/postprocess.h>     // Post processing flags

#pragma comment (lib, "assimp-vc142-mtd.lib")

void LoadFinish(const aiScene* scene)
{
	std::cout << "LoadFinish ! NumVertices : " << (*(scene->mMeshes))->mNumVertices << std::endl;
}

bool LoadModel(const std::string& pFile)
{
	// Create an instance of the Importer class
	Assimp::Importer importer;

	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll
	// probably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(pFile,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	// If the import failed, report it
	if (!scene)
	{
		std::cout << importer.GetErrorString() << std::endl;
		return false;
	}

	// Now we can access the file's contents.
	LoadFinish(scene);

	// We're done. Everything will be cleaned up by the importer destructor
	return true;
}
/*============= ASSIMP TEST =============*/

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

	//Shader shader = Shader(SHADER_PATH + "2. lighting/2.5 casters/cube_lighted.vert", SHADER_PATH + "2. lighting/2.5 casters/cube_lighted_directional.frag");
	//Shader shader = Shader(SHADER_PATH + "2. lighting/2.5 casters/cube_lighted.vert", SHADER_PATH + "2. lighting/2.5 casters/cube_lighted_point.frag");
	//Shader shader = Shader(SHADER_PATH + "2. lighting/2.5 casters/cube_lighted.vert", SHADER_PATH + "2. lighting/2.5 casters/cube_lighted_spot.frag");
	Shader shader = Shader(SHADER_PATH + "2. lighting/2.5 casters/cube_lighted.vert", SHADER_PATH + "2. lighting/2.5 casters/cube_lighted.frag");
	Shader lightCubeShader = Shader(SHADER_PATH + "2. lighting/light.vert", SHADER_PATH + "2. lighting/light.frag");

	unsigned int cubeVAO, VBO;
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int lightCubeVAO;
	glGenVertexArrays(2, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);

	// set attributes
	glm::vec3 lightPosition = glm::vec3(1.2f, 1.0f, 2.0f);
	std::string diffuseMapPath("resources/textures/container2.png");
	std::string specularMapPath("resources/textures/container2_specular.png");
	unsigned int diffuseMap = loadTexture(diffuseMapPath.c_str());
	unsigned int specularMap = loadTexture(specularMapPath.c_str());

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

		camera.setViewMatrix();

		// 1. draw cubes
		shader.use();
		shader.setVec3("viewPosition", camera.getPosition());
		shader.setInt("material.diffuseColor", 0);
		shader.setInt("material.specularColor", 1);
		shader.setFloat("material.shininess", 32.0f);
		// directional light
		shader.setVec3("directionalLight.direction", -0.2f, -1.0f, -0.3f);
		shader.setVec3("directionalLight.ambient", 0.33f, 0.33f, 0.33f);
		shader.setVec3("directionalLight.diffuse", 0.5f, 0.5f, 0.5f);
		shader.setVec3("directionalLight.specular", 1.0f, 1.0f, 1.0f);
		// point light
		//shader.setVec3("pointLight.position", lightPosition);
		//shader.setVec3("pointLight.ambient", 0.33f, 0.33f, 0.33f);
		//shader.setVec3("pointLight.diffuse", 0.5f, 0.5f, 0.5f);
		//shader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
		//shader.setFloat("pointLight.constant", 1.0f);
		//shader.setFloat("pointLight.linear", 0.09f);
		//shader.setFloat("pointLight.quadratic", 0.032f);
		// spot light
		shader.setVec3("spotLight.ambient", 0.33f, 0.33f, 0.33f);
		shader.setVec3("spotLight.diffuse", 0.5f, 0.5f, 0.5f);
		shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		shader.setVec3("spotLight.position", camera.getPosition());
		shader.setVec3("spotLight.direction", camera.getFront());
		shader.setFloat("spotLight.cutoff", glm::cos(glm::radians(12.5f)));
		shader.setFloat("spotLight.outerCutoff", glm::cos(glm::radians(17.5f)));
		shader.setFloat("spotLight.constant", 1.0f);
		shader.setFloat("spotLight.linear", 0.09f);
		shader.setFloat("spotLight.quadratic", 0.032f);

		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.getFov()), 4.0f/3.0f, 0.1f, 100.0f);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		glBindVertexArray(cubeVAO);
		for (unsigned int i = 0; i < 10; i++)
		{
			//std::cout << "??? here" << i << std::endl;
			glm::mat4 model(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			shader.setMat4("model", model);

			for (unsigned int j = 0; j < NR_POINT_LIGHTS; ++j)
			{
				std::string name = "pointLights[" + std::to_string(j) + std::string("].");
				//std::cout << name << std::endl;
				shader.setVec3(name + "position", pointLightPositions[j]);
				shader.setVec3(name + "ambient", 0.13f, 0.13f, 0.13f);
				shader.setVec3(name + "diffuse", 0.3f, 0.3f, 0.3f);
				shader.setVec3(name + "specular", 0.8f, 0.8f, 0.8f);
				shader.setFloat(name + "constant", 1.0f);
				shader.setFloat(name + "linear", 0.09f);
				shader.setFloat(name + "quadratic", 0.032f);

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		// draw point lights' position
		lightCubeShader.use();
		glBindVertexArray(lightCubeVAO);
		for (unsigned int i = 0; i < NR_POINT_LIGHTS; ++i)
		{
			glm::mat4 model(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.1f));
			lightCubeShader.setMat4("model", model);
			lightCubeShader.setMat4("view", view);
			lightCubeShader.setMat4("projection", projection);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		float currentTime = glfwGetTime();
		deltaFrameTime = currentTime - lastFrame;
		lastFrame = currentTime;

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &cubeVAO);
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
	// ASSIMP build test
	LoadModel("resources/objects/nanosuit/nanosuit.obj");
	
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
