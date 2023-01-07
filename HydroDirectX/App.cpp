#include "App.h"

App::App()
	:
	wnd( 800,600,"Hydro DirectX" )
{
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

void App::DoFrame()
{
	const float s = sin( timer.Peek() ) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer( s,s,1.0f );
	wnd.Gfx().DrawTestTriangle( -timer.Peek(),0.0f,0.0f );
	wnd.Gfx().DrawTestTriangle( timer.Peek(),wnd.mouse.GetPosX() / 400.0f - 1.0f, -wnd.mouse.GetPosY() / 300.0f + 1.0f);
	wnd.Gfx().EndFrame();
}
