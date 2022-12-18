#include "Window.h"

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevIstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd )
{
	//Create Window
	Window wnd( 800,600,L"Hydro DirectX" );

	//Message Handeling
	MSG msg;
	BOOL gResult;

	while ( (gResult = GetMessage( &msg, nullptr, 0, 0)) > 0 )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	if (gResult == -1)
	{
		return -1;
	}
	else
	{
		return (int)msg.wParam;
	}
}