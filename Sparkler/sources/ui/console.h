#ifndef CONSOLE_H
#define CONSOLE_H

#include "imgui.h"

static void HelpMarker(const char* desc);
static void showConsoleMenuBar();

// about LOGL Playground
static void showLoglPlaygroundInputCollapse();
static void showLoglPlaygroundAnalysisCollapse();

// about CAD Lab
static void showCadLabInputCollapse();

// enter function
void consoleWindow(bool* p_open);

#endif
