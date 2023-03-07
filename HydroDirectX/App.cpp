#include "App.h"
#include "Box.h"
#include "Cylinder.h"
#include "Pyramid.h"
#include "SkinnedBox.h"
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
	light( wnd.Gfx() )
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
			const DirectX::XMFLOAT3 mat = { cdist( rng ),cdist( rng ) ,cdist( rng ) };
			switch( sdist( rng ) )
			{
				case 0:
					return std::make_unique<Box>(
						gfx,rng,adist,ddist,
						odist,rdist,bdist,mat
						);
				case 1:
					return std::make_unique<Cylinder>(
						gfx,rng,adist,ddist,odist,
						rdist,bdist,tdist
						);
				case 2:
					return std::make_unique<Pyramid>(
						gfx,rng,adist,ddist,odist,
						rdist,tdist
						);
				case 3:
					return std::make_unique<SkinnedBox>(
						gfx, rng, adist, ddist,
						odist, rdist
						);
				default:
					assert( false && "impossible drawable option in factory" );
					return {};
			}
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_int_distribution<int> sdist{ 0,3 };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_real_distribution<float> cdist{ 0.0f,1.0f };
		std::uniform_int_distribution<int> tdist{ 3,30 };
	};

	Factory f( wnd.Gfx() );
	drawables.reserve( nDrawables );
	std::generate_n( std::back_inserter( drawables ),nDrawables,f );

	// init box pointers for editing instance parameters
	for( auto& pd : drawables )
	{
		if( auto pb = dynamic_cast<Box*>(pd.get()) )
		{
			boxes.push_back( pb );
		}
	}

	wnd.Gfx().SetProjection( DirectX::XMMatrixPerspectiveLH( 1.0f,3.0f / 4.0f,0.5f,40.0f ) );
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().SetCamera( camera.GetMatrix() );
	wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
	light.Bind( wnd.Gfx(),camera.GetMatrix() );
	
	for( auto& d : drawables )
	{
		d->Update( wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt );
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
	// imgui window to open box windows
	if( ImGui::Begin( "Boxes" ) )
	{
		using namespace std::string_literals;
		const auto preview = comboBoxIndex ? std::to_string( *comboBoxIndex ) : "Choose a box..."s;
		if( ImGui::BeginCombo( "Box Number", preview.c_str() ) )
		{
			for( int i = 0; i < boxes.size(); i++ )
			{
				const bool selected = comboBoxIndex.has_value() ? *comboBoxIndex == i : false;
				if( ImGui::Selectable( std::to_string( i ).c_str(), selected ) )
				{
					comboBoxIndex = i;
				}
				if( selected )
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		if( ImGui::Button( "Spawn Control Window" ) && comboBoxIndex )
		{
			boxControlIds.insert( *comboBoxIndex );
			comboBoxIndex.reset();
		}
	}
	ImGui::End();
	// imgui box attribute control windows
	for( auto id : boxControlIds )
	{
		boxes[id]->SpawnControlWindow( id, wnd.Gfx() );
	}

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

