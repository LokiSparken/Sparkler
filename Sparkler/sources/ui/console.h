#ifndef CONSOLE_H
#define CONSOLE_H

#include "imgui.h"

static void HelpMarker(const char* desc);
static void showConsoleMenuBar();
static void showConsoleInputCollapse();
static void showConsoleInspectorCollapse();
void consoleWindow(bool* p_open);

#endif
