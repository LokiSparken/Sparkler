#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "..\misc\fonts\IconsFontAwesome5.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <iostream>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

const char* vertexShaderCode = 
"#version 330 core\n"
"layout (location = 0) in vec3 pos;\n"
"layout (location = 1) in vec3 col;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"    color = vec4(col.x, col.y, col.z, 1.0);\n"
"    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);\n"
"}\0";

const char* fragmentShaderCode = 
"#version 330 core\n"
"in vec4 color;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"    fragColor = color;\n"
"}\n\0";

const char* fragmentShaderPracticeCode =
"#version 330 core\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"    fragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);\n"
"}\n\0";

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char**)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	const char* glsl_version = "#version 330";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Sparkler", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Initialize OpenGL loader
	bool err = gladLoadGL() == 0;
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

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
	bool show_demo_window = false;
	bool show_cglc_window = false;
	bool show_logl_window = false;
	bool show_rtlb_window = false;
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Poll and handle events (inputs, window resize, etc.)
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Demo window
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// Create window - Console
		{
			ImGui::Begin(ICON_FA_STAR " Console");
			
			if (ImGui::Button("Clear"))
				clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
			
			if (ImGui::Button("CG in Last Century"))
				show_cglc_window ^= 1;

			if (ImGui::Button("LearnOpenGL Playground"))
				show_logl_window ^= 1;

			//if (ImGui::Button("Ray Tracer Lab"))
			//	show_rtlb_window ^= 1;

			ImGui::Text("Average: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// Create window - LearnOpenGL
		if (show_logl_window)
		{
			ImGui::Begin(ICON_FA_STAR " LearnOpenGL");
			if (ImGui::Button("Hello, Triangle. w"))
			{
				clear_color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
			}
			ImGui::End();
		}

		// Hello, Triangle
		// Step 1. Define vertices data and store into VRAM

		// Step 2. Define the vertex shader - vertexShaderCode
		// Step 3. Define the fragment shader - fragmentShaderCode
		// Step 4. Compile the shaders
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		unsigned int fragmentShaderPractice = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderCode, nullptr);
		glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);
		glShaderSource(fragmentShaderPractice, 1, &fragmentShaderPracticeCode, nullptr);
		glCompileShader(vertexShader);
		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
					  << infoLog
					  << std::endl;
		}
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
					  << infoLog
					  << std::endl;
		}
		glCompileShader(fragmentShaderPractice);
		glGetShaderiv(fragmentShaderPractice, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShaderPractice, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
					  << infoLog
					  << std::endl;
		}
		// Step 5. Link shader objects to shader program
		unsigned int shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n"
  					  << infoLog
					  << std::endl;
		}
		glDeleteShader(fragmentShader);

		unsigned int shaderProgramPractice = glCreateProgram();
		glAttachShader(shaderProgramPractice, vertexShader);
		glAttachShader(shaderProgramPractice, fragmentShaderPractice);
		glLinkProgram(shaderProgramPractice);
		glGetProgramiv(shaderProgramPractice, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n"
				  	  << infoLog
					  << std::endl;
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShaderPractice);


		// Step 6. Link vertex attribute
		float vertices[] = { 
								-0.5f, -0.5f, 0.0f,
								 1.0f,  0.0f, 0.0f,
								 0.5f, -0.5f, 0.0f,
								 0.0f,  1.0f, 0.0f,
								 0.0f,  0.5f, 0.0f,
								 0.0f,  0.0f, 1.0f
						   };

		float smallTriangle[] = {
									0.0f, 0.5f, 0.0f,
									0.0f, 0.0f, 1.0f,
								   -0.2f, 0.8f, 0.0f,
									0.0f, 1.0f, 0.0f,
									0.2f, 0.8f, 0.0f,
									1.0f, 0.0f, 0.0f
								};

		float programPracticeTriangle[] = {
											0.0f, 0.5f, 0.0f,
											0.0f, 0.0f, 1.0f,
											0.3f, 0.8f, 0.0f,
											0.0f, 0.0f, 1.0f,
											0.3f, 0.2f, 0.0f,
											0.0f, 0.0f, 1.0f
										  };

		unsigned int VBO, smallTriangleVBO, practiceVBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		unsigned int VAO, smallTriangleVAO, practiceVAO;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
		glEnableVertexAttribArray(1);

		glGenBuffers(2, &smallTriangleVBO);
		glBindBuffer(GL_ARRAY_BUFFER, smallTriangleVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(smallTriangle), smallTriangle, GL_STATIC_DRAW);
		glGenVertexArrays(2, &smallTriangleVAO);
		glBindVertexArray(smallTriangleVAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
		glEnableVertexAttribArray(1);

		glGenBuffers(3, &practiceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, practiceVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(programPracticeTriangle), programPracticeTriangle, GL_STATIC_DRAW);
		glGenVertexArrays(3, &practiceVAO);
		glBindVertexArray(practiceVAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
		glEnableVertexAttribArray(1);

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw Triangle
		glUseProgram(shaderProgram);
		glBindVertexArray(smallTriangleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glUseProgram(shaderProgramPractice);
		glBindVertexArray(practiceVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

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
