#include "Window.h"

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevIstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd )
{
	try
	{
		//Create Window
		Window wnd( 800,600, "Hydro DirectX" );

		//Message Handeling
		MSG msg;
		BOOL gResult;

		while((gResult = GetMessage( &msg,nullptr,0,0 )) > 0)
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		if(gResult == -1)
		{
			return -1;
		}
		else
		{
			return (int)msg.wParam;
		}
	}
	catch(const HydroException& e)
	{
		MessageBox( nullptr,e.what(),e.GetType(),MB_OK | MB_ICONEXCLAMATION );
	}
	catch(const std::exception& e)
	{
		MessageBox( nullptr,e.what(),"Standrard Exception",MB_OK | MB_ICONEXCLAMATION );
	}
	catch( ... )
	{
		MessageBox( nullptr,"No details available","Unkown Exception", MB_OK | MB_ICONEXCLAMATION );
	}
	return -1;
}