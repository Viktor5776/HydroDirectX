#pragma once
#include "Window.h"
#include "HydroTimer.h"
#include "ImguiManager.h"
#include "Camera.h"

class App
{
public:
	App();
	//Message Loop
	int Go();
	~App();
private:
	void DoFrame();
private:
	ImguiManager imgui;
	Window wnd;
	HydroTimer timer;
	Camera camera;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	float speed_factor = 1.0f;
	static constexpr size_t nDrawables = 180;
};