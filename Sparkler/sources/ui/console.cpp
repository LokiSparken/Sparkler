#include "console.h"

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void consoleWindow(bool* p_open)
{
	IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_MenuBar;

	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

	// Main body of the 'Console' window starts here.
	if (!ImGui::Begin("Console", p_open, window_flags))
	{
		ImGui::End();
		return;
	}

	ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

	showConsoleMenuBar();
	
	static bool logl = false;
	static bool cad = false;

	ImGui::TextColored(ImVec4(0.39f, 0.58f, 0.93f, 1.0f), "Choose Playground ");
	if (ImGui::RadioButton("LOGL", logl)) logl = true;
	if (logl && cad) cad = false;
	ImGui::SameLine(); 
	if (ImGui::RadioButton("CAD", cad)) cad = true;
	if (cad && logl) logl = false;

	ImGui::Spacing();


	if (logl)
	{
		ImGui::TextColored(ImVec4(0.39f, 0.58f, 0.93f, 1.0f), "LOGL Playground Inspector");
		showLoglPlaygroundInputCollapse();
		ImGui::Spacing();
		showLoglPlaygroundAnalysisCollapse();
	}
	
	if (cad)
	{
		ImGui::TextColored(ImVec4(0.39f, 0.58f, 0.93f, 1.0f), "CAD Lab Inspector");
		showCadLabInputCollapse();
	}
	

	ImGui::Spacing();
	if (ImGui::Button("Render"))
	{

	}

	ImGui::End();
}

static void showLoglPlaygroundInputCollapse()
{
	if (ImGui::CollapsingHeader("Input Settings"))
	{
		//ImGui::TextColored(ImVec4(0.39f, 0.58f, 0.93f, 1.0f), "Camera");
		if (ImGui::TreeNode("Camera"))
		{
			{
				ImGui::InputFloat3("Position", cameraPosition);
			}

			{
				//ImGui::SliderFloat("Move Speed", &Camera::moveSpeed, 0.0f, 1.0f, "");
				ImGui::SliderFloat("Move Speed", &cameraMoveSpeed, 0.0f, 100.0f, "%.2f");
			}

			ImGui::TreePop();
		}

		ImGui::Separator();

		//ImGui::TextColored(ImVec4(0.39f, 0.58f, 0.93f, 1.0f), "Object");
		if (ImGui::TreeNode("Object"))
		{
			{
				ImGui::InputFloat3("Position", cameraPosition);
			}

			{
				static float objectDiffuse[3] = { 0.39f, 0.58f, 0.93f };
				ImGui::ColorEdit3("Diffuse Color", objectDiffuse);
				ImGui::SameLine(); HelpMarker(
					"TODO: Change color for nothing now.\n"
					"Click on the color square to open a color picker.\n"
					"Click and hold to use drag and drop.\n"
					"Right-click on the color square to show options.\n"
					"CTRL+click on individual component to input value.\n");
			}

			{
				static float objectSpecular[3] = { 0.39f, 0.58f, 0.93f };
				ImGui::ColorEdit3("Specular Color", objectSpecular);
				ImGui::SameLine(); HelpMarker(
					"TODO: Change color for nothing now.\n"
					"Click on the color square to open a color picker.\n"
					"Click and hold to use drag and drop.\n"
					"Right-click on the color square to show options.\n"
					"CTRL+click on individual component to input value.\n");
			}

			ImGui::TreePop();
		}

		ImGui::Separator();

		//ImGui::TextColored(ImVec4(0.39f, 0.58f, 0.93f, 1.0f), "Light");
		if (ImGui::TreeNode("Light"))
		{
			ImGui::Text("List of lights: ");

			ImGui::Button("Add new light");
			
			ImGui::InputFloat3("Position", cameraPosition);

			{
				static float lightDiffuse[3] = { 1.0f, 1.0f, 1.0f };
				ImGui::ColorEdit3("Diffuse Color", lightDiffuse);
				ImGui::SameLine(); HelpMarker(
					"TODO: Change color for nothing now.\n"
					"Click on the color square to open a color picker.\n"
					"Click and hold to use drag and drop.\n"
					"Right-click on the color square to show options.\n"
					"CTRL+click on individual component to input value.\n");
			}

			{
				static float lightSpecular[3] = { 1.0f, 1.0f, 1.0f };
				ImGui::ColorEdit3("Specular Color", lightSpecular);
				ImGui::SameLine(); HelpMarker(
					"TODO: Change color for nothing now.\n"
					"Click on the color square to open a color picker.\n"
					"Click and hold to use drag and drop.\n"
					"Right-click on the color square to show options.\n"
					"CTRL+click on individual component to input value.\n");
			}
			
			ImGui::TreePop();
		}
	}
}

static void showLoglPlaygroundAnalysisCollapse()
{
	if (ImGui::CollapsingHeader("Analysis"))
	{
		//ImGui::TextColored(ImVec4(0.39f, 0.58f, 0.93f, 1.0f), "Average Speed");
		ImGui::Text("Average Speed: ");
		ImGui::SameLine();
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::Text("Objects: ");
		ImGui::Text("Triangles: ");
		ImGui::Text("Draw Call: ");
	}
}

static void showCadLabInputCollapse()
{
	if (ImGui::CollapsingHeader("Input Settings"))
	{
		ImGui::LabelText("Description", "Input");

		{
			const char* items[] = { "Cube", "Sphere" };
			static int item_current = 0;
			ImGui::Combo("Object", &item_current, items, IM_ARRAYSIZE(items));
			ImGui::SameLine(); 
			HelpMarker("Select the object to render.");
		}

		{

		}
	}
}

static void showConsoleMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Quit", "Alt+F4")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("test")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("About Sparkler")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}