#include "App.h"
#include "Box.h"
#include "HydroMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <memory>
#include <algorithm>

GDIPlusManager gdipm;

App::App()
	:
	wnd( 800,600,"Hydro DirectX" ),
	light(wnd.Gfx())
{
	class Factory
	{
	public:
		Factory( Graphics & gfx )
			:
			gfx( gfx )
		{}
		std::unique_ptr<Drawable> operator()()
		{
			return std::make_unique<Box>(
					gfx,rng,adist,ddist,
					odist,rdist,bdist
			);
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
	};

	Factory f( wnd.Gfx() );
	drawables.reserve( nDrawables );
	std::generate_n( std::back_inserter( drawables ),nDrawables,f );

	wnd.Gfx().SetProjection( DirectX::XMMatrixPerspectiveLH( 1.0f,3.0f / 4.0f,0.5f,40.0f ) );
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().SetCamera( camera.GetMatrix() );
	wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
	light.Bind( wnd.Gfx() );
	
	for( auto& d : drawables )
	{
		d->Update( wnd.kbd.KeyIsPressed(	VK_SPACE) ? 0.0f : dt );
		d->Draw( wnd.Gfx() );
	}
	light.Draw( wnd.Gfx() );

	//Imgui Window to Control Simulation Speed
	if( ImGui::Begin( "Simulation Speed" ) )
	{
		ImGui::SliderFloat( "Speed Factor",&speed_factor,-5.0f,5.0f );
		ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)",1000.0f / ImGui::GetIO().Framerate,ImGui::GetIO().Framerate );
		ImGui::Text( "Status: %s",wnd.kbd.KeyIsPressed( VK_SPACE ) ? "PAUSED" : "RUNNING (hold spacebar to pause)" );
	}
	ImGui::End();

	//Imgui Window to Control Camera And Light
	camera.SpawnControlWindow();
	light.SpawnControlWindow();

	//Present Frame
	wnd.Gfx().EndFrame();
}

int App::Go()
{
	while( true )
	{
		//Proccess Messages
		if( const auto ecode = Window::ProcessMessages() )
		{
			//Return With Exit Code of ecode
			return ecode.value();
		}
		DoFrame();
	}
	
}

App::~App()
{}

