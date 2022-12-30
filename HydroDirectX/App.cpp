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
	while( !wnd.mouse.IsEmpty() )
			{
				auto e = wnd.mouse.Read();
				if( e.GetType() == Mouse::Event::Type::Move )
				{
					std::ostringstream oss;
					oss << "Mousepos ( " << e.GetPosX() << ", " << e.GetPosY() << " )";
					wnd.SetTitle( oss.str() );
				}
			}

	const float s = sin( timer.Peek() ) / 2.0 + 0.3f;
	const float c = cos( timer.Peek() ) / 2.0 + 0.3f;
	wnd.Gfx().ClearBuffer( s,1.0f,s );
	wnd.Gfx().EndFrame();
}
