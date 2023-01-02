#pragma once
#include "Window.h"
#include "HydroTimer.h"

class App
{
public:
	App();
	//Message Loop
	int Go();
private:
	void DoFrame();
private:
	Window wnd;
	HydroTimer timer;
	float y = 0.0f;
};