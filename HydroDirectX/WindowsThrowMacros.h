#pragma once

//Error Exception Marco
#define HYWND_EXCEPT( hr ) Window::HrException( __LINE__,__FILE__,(hr) )
#define HYWND_LAST_EXCEPT() Window::HrException( __LINE__,__FILE__,GetLastError() )
#define HYWND_NOGFX_EXCEPT() Window::NoGfxException( __LINE__,__FILE__ )