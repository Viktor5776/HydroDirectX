#pragma once
#include "Window.h"
#include "HydroTimer.h"

#include "Sprite.h"

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
	Window wnd;
	HydroTimer timer;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	Sprite sprite;
	static constexpr size_t nDrawables = 180;
};