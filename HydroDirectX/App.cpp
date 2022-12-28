#include "App.h"
#include <sstream>
#include <iomanip>

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
	const float t = timer.Peek();
	std::ostringstream oss;
	oss << "Time elapsed: " << std::setprecision( 1 ) << std::fixed << t << "s";
	wnd.SetTitle( oss.str() );
}
