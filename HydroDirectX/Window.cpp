#include "Window.h"
#include "WindowsThrowMacros.h"
#include "resource.h"
#include "imgui/imgui_impl_win32.h"
#include <sstream>

//Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst( GetModuleHandle( nullptr ) )
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof( wc );
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage( hInst,MAKEINTRESOURCE( IDI_ICON1 ),IMAGE_ICON,32,32,0 ));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage( hInst,MAKEINTRESOURCE( IDI_ICON1 ),IMAGE_ICON,16,16,0 ));;
	RegisterClassEx( &wc );
}

Window::WindowClass::~WindowClass()
{
	ImGui_ImplWin32_Shutdown();
	UnregisterClass( wndClassName,GetInstance() );
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

//Window Stuff
Window::Window( int width,int height,const char* name )
	:
	width( width ),
	height( height )
{
	//Calculat Window Size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if( AdjustWindowRect( &wr,WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,FALSE ) == 0 )
	{
		throw HYWND_LAST_EXCEPT();
	}

	//Create Window and Get hWnd
	hWnd = CreateWindow(
		WindowClass::GetName(),name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,CW_USEDEFAULT,
		wr.right - wr.left,wr.bottom - wr.top,
		nullptr,nullptr,WindowClass::GetInstance(),this
	);

	//Check for Error
	if( hWnd == nullptr )
	{
		throw HYWND_LAST_EXCEPT();
	}

	//Show Window ( Defualt Behaivor is Hidden )
	ShowWindow( hWnd,SW_SHOWDEFAULT );

	//Init ImGui Win32 Implementation
	ImGui_ImplWin32_Init( hWnd );

	//Create Graphics Object
	pGfx = std::make_unique<Graphics>( hWnd );
}

Window::~Window()
{
	DestroyWindow( hWnd );
}

void Window::SetTitle( const std::string title )
{
	if( SetWindowText( hWnd,title.c_str() ) == 0 )
	{
		throw HYWND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessages() noexcept
{
	MSG msg;

	while( PeekMessage( &msg,nullptr,0,0,PM_REMOVE ) )
	{
		if( msg.message == WM_QUIT )
		{
			//Return Exit Code When Quting
			return (int)msg.wParam;
		}

		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	//Return Empty Optinal When Not Quiting
	return {};
}

Graphics& Window::Gfx()
{
	if( !pGfx )
	{
		throw HYWND_NOGFX_EXCEPT();
	}
	return *pGfx;
}

LRESULT CALLBACK Window::HandleMsgSetup( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam ) noexcept
{
	//WM_NCCREATE msg Gets Called When We Use CreateWindow()
	if( msg == WM_NCCREATE )
	{
		//Get ptr To Window Class
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		//Set WinAPI to store ptr to Window Class
		SetWindowLongPtr( hWnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(pWnd) );
		//Set Message Proc to Normal (non-setup) Handler
		SetWindowLongPtr( hWnd,GWLP_WNDPROC,reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk) );
		//Forward message to Window Class Handler
		return pWnd->HandleMsg( hWnd,msg,wParam,lParam );
	}
	//Messages before WM_NCCREATE Get Forwarded to Default Handler
	return DefWindowProc( hWnd,msg,wParam,lParam );
}

LRESULT CALLBACK Window::HandleMsgThunk( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam ) noexcept
{
	//Retrive ptr to Window Class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr( hWnd,GWLP_USERDATA ));
	//Forward message to Window Class Handler
	return pWnd->HandleMsg( hWnd,msg,wParam,lParam );
}

LRESULT Window::HandleMsg( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam ) noexcept
{
	if( ImGui_ImplWin32_WndProcHandler( hWnd,msg,wParam,lParam ) )
	{
		return true;
	}
	const auto& imio = ImGui::GetIO();


	switch( msg )
	{
		case WM_CLOSE:
			PostQuitMessage( 0 );
			return 0;
			//Clear Keystate When Window Loses Focus
		case WM_KILLFOCUS:
			kbd.ClearState();
			break;
			/********** KEYBOARD MESSAGES **********/
		case WM_KEYDOWN:
			//SYSKEY Commands Need to be Handeld For ALT ( VM_MENU ) and F10
		case WM_SYSKEYDOWN:
			//Remove Message if Imgui Wants to Capture
			if( imio.WantCaptureKeyboard )
			{
				break;
			}
			//Filter Auto Repeat
			if( !(lParam & 0x40000000) || kbd.AutorepeatIsEnabled() )
			{
				kbd.OnKeyPressed( static_cast<unsigned char>(wParam) );
			}
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			//Remove Message if Imgui Wants to Capture
			if( imio.WantCaptureKeyboard )
			{
				break;
			}
			kbd.OnKeyReleased( static_cast<unsigned char>(wParam) );
			break;
		case WM_CHAR:
			//Remove Message if Imgui Wants to Capture
			if( imio.WantCaptureKeyboard )
			{
				break;
			}
			kbd.OnChar( static_cast<unsigned char>(wParam) );
			break;
			/******** END KEYBOARD MESSAGES ********/

			/********** Mouse MESSAGES **********/
		case WM_MOUSEMOVE:
			//Remove Message if Imgui Wants to Capture
			if( imio.WantCaptureKeyboard )
			{
				break;
			}
			if( GetFocus() == hWnd )
			{
				const POINTS pt = MAKEPOINTS( lParam );
				//In Window
				if( pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height )
				{
					mouse.OnMouseMove( pt.x,pt.y );
					if( !mouse.IsInWindow() )
					{
						SetCapture( hWnd );
						mouse.OnMouseEnter();
					}
				}
				//Not in Window ( Maintain Capture if Button is Down )
				else
				{
					if( wParam & (MK_LBUTTON | MK_RBUTTON) )
					{
						mouse.OnMouseMove( pt.x,pt.y );
					}
					//Button Up ( Release Capture )
					else
					{
						ReleaseCapture();
						mouse.OnMouseLeave();

					}
				}
			}
			break;
		case WM_LBUTTONDOWN:
			//Remove Message if Imgui Wants to Capture
			if( imio.WantCaptureKeyboard )
			{
				break;
			}
			mouse.OnLeftPressed();
			break;
		case WM_LBUTTONUP:
			//Remove Message if Imgui Wants to Capture
			if( imio.WantCaptureKeyboard )
			{
				break;
			}
			mouse.OnLeftRelease();
			break;
		case WM_RBUTTONDOWN:
			//Remove Message if Imgui Wants to Capture
			if( imio.WantCaptureKeyboard )
			{
				break;
			}
			mouse.OnRightPressed();
			break;
		case WM_RBUTTONUP:
			//Remove Message if Imgui Wants to Capture
			if( imio.WantCaptureKeyboard )
			{
				break;
			}
			mouse.OnRightRelease();
			break;
		case WM_MOUSEWHEEL:
			//Remove Message if Imgui Wants to Capture
			if( imio.WantCaptureKeyboard )
			{
				break;
			}
			mouse.OnWheelDelta( GET_WHEEL_DELTA_WPARAM( wParam ) );
			break;
		case WM_MBUTTONDOWN:
			//Remove Message if Imgui Wants to Capture
			if( imio.WantCaptureKeyboard )
			{
				break;
			}
			mouse.OnWheelPressed();
			break;
	}

	return DefWindowProc( hWnd,msg,wParam,lParam );
}

//Window Exception Stuff

std::string Window::Exception::TranslateErrorCode( HRESULT hr ) noexcept
{
	char* pMsgBuf = nullptr;
	const DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,hr,MAKELANGID( LANG_NEUTRAL,SUBLANG_DEFAULT ),
		reinterpret_cast<LPSTR>(&pMsgBuf),0,nullptr
	);
	if( nMsgLen == 0 )
	{
		return "Unidentified error code";

	}
	std::string errorString = pMsgBuf;
	LocalFree( pMsgBuf ); //THIS
	return errorString;
}

Window::HrException::HrException( int line,const char* file,HRESULT hr ) noexcept
	:
	Exception( line,file ),
	hr( hr )
{}

const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
	return "Hydro Window Exception";
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}


std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exception::TranslateErrorCode( hr );
}


const char* Window::NoGfxException::GetType() const noexcept
{
	return "Hydro Window Exception [No Graphics]";
}