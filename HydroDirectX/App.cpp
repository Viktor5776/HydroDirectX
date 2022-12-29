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
	const float s = sin( timer.Peek() ) / 2.0 + 0.3f;
	const float c = cos( timer.Peek() ) / 2.0 + 0.3f;
	wnd.Gfx().ClearBuffer( s,1.0f,s );
	wnd.Gfx().EndFrame();
}
