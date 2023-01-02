#include "App.h"

App::App()
	:
	wnd( 1920,1048,"Hydro DirectX" )
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
					y = (1.0f - e.GetPosY() / 1048.0f) + (e.GetPosY() / 1048.0f) * -1.0f;
				}
			}

	wnd.Gfx().ClearBuffer( 0.0f,0.15f,0.2f );
	wnd.Gfx().DrawTestTriangle(y);
	wnd.Gfx().EndFrame();
}
