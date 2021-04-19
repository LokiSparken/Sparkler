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

	showConsoleInputCollapse();
	ImGui::Spacing();
	showConsoleInspectorCollapse();

	ImGui::End();
}

static void showConsoleInputCollapse()
{
	if (ImGui::CollapsingHeader("Input Settings"))
	{
		ImGui::TextColored(ImVec4(0.39f, 0.58f, 0.93f, 1.0f), "Object");

		ImGui::Separator();

		ImGui::TextColored(ImVec4(0.39f, 0.58f, 0.93f, 1.0f), "Light");
		{
			{
				static float ambient[3] = { 0.39f, 0.58f, 0.93f };
				ImGui::ColorEdit3("Ambient", ambient);
				ImGui::SameLine(); HelpMarker(
					"TODO: Change color for nothing now.\n"
					"Click on the color square to open a color picker.\n"
					"Click and hold to use drag and drop.\n"
					"Right-click on the color square to show options.\n"
					"CTRL+click on individual component to input value.\n");
			}
		}
	}
}

static void showConsoleInspectorCollapse()
{
	if (ImGui::CollapsingHeader("Inspector"))
	{
		ImGui::TextColored(ImVec4(0.39f, 0.58f, 0.93f, 1.0f), "Average Speed");
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
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