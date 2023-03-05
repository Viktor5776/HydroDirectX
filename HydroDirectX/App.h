#pragma once
#include "Window.h"
#include "HydroTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include <set>

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
	PointLight light;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	std::vector<class Box*> boxes;
	float speed_factor = 1.0f;
	static constexpr size_t nDrawables = 180;
	std::optional<int> comboBoxIndex;
	std::set<int> boxControlIds;
};