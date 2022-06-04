#pragma once
#include "sys_ui.h"
#include "windows_render.h"
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "opengl32.lib")
#ifndef __llvm__
#pragma comment(linker,"\"/manifestdependency:type='win32'  \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0'  \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' \
language='*'\"")
#endif

str sys::dialog (str title, str text, sys::choice choice, void* handle)
{
    UINT type = MB_OK;
    switch (choice) {
    case sys::choice::ok:                  type = MB_OK                ; break;
    case sys::choice::ok_cancel:           type = MB_OKCANCEL          ; break;
    case sys::choice::yes_no:              type = MB_YESNO             ; break;
    case sys::choice::yes_no_cancel:       type = MB_YESNOCANCEL       ; break;
    case sys::choice::abort_retry_ignore:  type = MB_ABORTRETRYIGNORE  ; break;
    case sys::choice::cancel_try_continue: type = MB_CANCELTRYCONTINUE ; break;
    case sys::choice::retry_cancel:        type = MB_RETRYCANCEL       ; break;
    }

    type |= MB_APPLMODAL | MB_ICONWARNING;

    auto rc = ::MessageBox((HWND)(handle), text.c_str(), title.c_str(), type);

    switch (rc) {
    case IDOK       : return "ok";
    case IDCANCEL   : return "cancel";
    case IDABORT    : return "abort";
    case IDRETRY    : return "retry";
    case IDIGNORE   : return "ignore";
    case IDYES      : return "yes";
    case IDNO       : return "no";
    case IDTRYAGAIN : return "try again";
    case IDCONTINUE : return "continue";
    default: return "";
    }
}

void sys::mouse::image(str image)
{
    ::SetCursor(::LoadCursor(NULL,
        image == "hand" ? IDC_HAND :
        image == "noway" ? IDC_NO :
        image == "no way" ? IDC_NO :
        image == "editor" ? IDC_IBEAM :
        image == "vertical splitter" ? IDC_SIZEWE :
        image == "horizontal splitter" ? IDC_SIZENS :
        IDC_ARROW
    ));
}

auto sys::mouse::position() -> xy
{
    POINT p; ::GetCursorPos(&p); return xy{p.x, p.y};
}
void sys::mouse::position(xy p)
{
    ::SetCursorPos(p.x, p.y);
}

// https://blog.keyman.com/2008/06/robust-key-mess/

static str wm_key (WPARAM wparam, LPARAM, bool down)
{
    str s;
    if (0x30 <= wparam && wparam <= 0x39) s = '0' + (char)wparam-0x30; else
    if (0x41 <= wparam && wparam <= 0x5A) s = 'A' + (char)wparam-0x41; else
    if (0x70 <= wparam && wparam <= 0x7B) s = "F" + std::to_string(wparam-0x70+1); else
    switch(wparam){
    case VK_BACK    : s = "backspace"; break;
    case VK_TAB     : s = "tab"; break;
    case VK_RETURN  : s = "enter"; break;
    case VK_ESCAPE  : s = "escape"; break;
    case VK_INSERT  : s = "insert"; break;
    case VK_DELETE  : s = "delete"; break;
    case VK_SPACE   : s = "space"; break;
    case VK_SNAPSHOT: s = "print screen"; break;

    case VK_PRIOR   : s = "page up"; break;
    case VK_NEXT    : s = "page down"; break;
    case VK_END     : s = "end"; break;
    case VK_HOME    : s = "home"; break;
    case VK_LEFT    : s = "left"; break;
    case VK_RIGHT   : s = "right"; break;
    case VK_UP      : s = "up"; break;
    case VK_DOWN    : s = "down"; break;

    case VK_OEM_PLUS    : s = "+"; break;
    case VK_OEM_MINUS   : s = "-"; break;
    case VK_OEM_COMMA   : s = ","; break;
    case VK_OEM_PERIOD  : s = "."; break;

    case VK_MENU    : sys::keyboard::alt   = down; break;
    case VK_SHIFT   : sys::keyboard::shift = down; break;
    case VK_CONTROL : sys::keyboard::ctrl  = down; break;
    }
    if (s == "") return "";
    if (sys::keyboard::shift) s = "shift+" + s;
    if (sys::keyboard::alt  ) s =   "alt+" + s;
    if (sys::keyboard::ctrl ) s =  "ctrl+" + s;
    return s;
}
static str wm_chr (WPARAM wparam, LPARAM)
{
    if (wparam < 32) return "";
    if (sys::keyboard::alt) return "";
    if (sys::keyboard::ctrl) return "";
    std::wstring s; s += (wchar_t)wparam;
    return unwinstr(s);
}

LRESULT CALLBACK WindowProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    sys::window* win = (sys::window*)(::GetWindowLongPtr(hwnd, GWLP_USERDATA));

    int LX = GET_X_LPARAM(lparam);
    int LY = GET_Y_LPARAM(lparam);

    const str L = "left";
    const str M = "middle";
    const str R = "right";

    POINT p{0,0}; ClientToScreen(hwnd, &p); xy o {p.x, p.y};

    switch (msg) {
    case WM_COMMAND: if (wparam == 11111) win->on_timing(); break;

    case WM_MOUSEMOVE     : win->mouse_on_move  (xy(LX,LY)); break;
    case WM_MOUSEWHEEL    : win->mouse_on_wheel (xy(LX,LY) - o, (short)HIWORD(wparam)); break;
    case WM_LBUTTONDOWN   : win->mouse_on_press (xy(LX,LY), L, true ); SetCapture(hwnd); break;
    case WM_LBUTTONUP     : win->mouse_on_press (xy(LX,LY), L, false); ReleaseCapture(); break;
    case WM_MBUTTONDOWN   : win->mouse_on_press (xy(LX,LY), M, true ); break;
    case WM_MBUTTONUP     : win->mouse_on_press (xy(LX,LY), M, false); break;
    case WM_RBUTTONDOWN   : win->mouse_on_press (xy(LX,LY), R, true ); break;
    case WM_RBUTTONUP     : win->mouse_on_press (xy(LX,LY), R, false); break;
    case WM_CAPTURECHANGED: win->mouse_on_leave (); break;
    case WM_MOUSELEAVE    : win->mouse_on_leave (); break;

    case WM_SETFOCUS      : win->keyboard_on_focus (true); break;
    case WM_KILLFOCUS     : win->keyboard_on_focus (false); break;
    case WM_SYSKEYDOWN    : win->keyboard_on_press (wm_key(wparam, lparam, true ), true ); break;
    case WM_SYSKEYUP      : win->keyboard_on_press (wm_key(wparam, lparam, false), false); break;
    case WM_KEYDOWN       : win->keyboard_on_press (wm_key(wparam, lparam, true ), true ); break;
    case WM_KEYUP         : win->keyboard_on_press (wm_key(wparam, lparam, false), false); break;
    case WM_CHAR          : win->keyboard_on_input (wm_chr(wparam, lparam)); break;

    case WM_SIZE:
    {
        static bool minimized = false;
        if(wparam == SIZE_MINIMIZED) { if (!minimized) win->on_pause (); minimized = true; break; }
        if(wparam != SIZE_MINIMIZED) { if (!minimized) win->on_resume(); minimized = false; }
        win->on_resize(xy(LOWORD(lparam), HIWORD(lparam)));
        break;
    }
    case WM_CREATE :
        win = (sys::window*)((CREATESTRUCT*)lparam)->lpCreateParams;
        ::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(win));
        win->native_handle1 = hwnd;
        win->on_start();
        return 0;

    case WM_DESTROY:
        win->on_finish();
        PostQuitMessage(0);
        return 0;

    default: return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    return 0;
}

LRESULT CALLBACK PixWindowProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    auto rc = msg == WM_PAINT ? 0 : WindowProc(hwnd, msg, wparam, lparam);

    sys::window* win = (sys::window*)(::GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (msg) {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        int x = ps.rcPaint.left;
        int y = ps.rcPaint.top;
        int w = ps.rcPaint.right - x;
        int h = ps.rcPaint.bottom - y;
        int W = win->image.size.x;

        BITMAPINFO bi;
        ZeroMemory(&bi,              sizeof(bi));
        bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth       = W;
        bi.bmiHeader.biHeight      = -h;
        bi.bmiHeader.biPlanes      = 1;
        bi.bmiHeader.biBitCount    = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        bi.bmiHeader.biSizeImage   = W*h*4;

        auto p = &win->image(0,y);
        SetDIBitsToDevice(hdc, x,y,w,h, x,0,0,h, p, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
        EndPaint(hwnd, &ps);
        return 0;
    }
    }

    if (win // not yet before WM_CREATE
    &&  win->image.size.x > 0 // not yet on first WM_SETFOCUS
    &&  win->image.size.y > 0)
        win->update();

    return rc;
}

LRESULT CALLBACK GpuWindowProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    auto rc = msg == WM_PAINT ? 0 : WindowProc(hwnd, msg, wparam, lparam);

    sys::window* win = (sys::window*)(::GetWindowLongPtr(hwnd, GWLP_USERDATA));

    HGLRC handle = win ? (HGLRC)(win->native_handle2) : NULL;

    switch (msg) {
    case WM_CREATE :
    {
        PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,                      // version number
            PFD_DRAW_TO_WINDOW |    // support window
            PFD_SUPPORT_OPENGL |    // support OpenGL
            PFD_DOUBLEBUFFER,       // double buffered
            PFD_TYPE_RGBA,          // rgba type
            24,                     // 24-bit color depth
            0, 0, 0, 0, 0, 0,       // color bits ignored
            0,                      // no alpha buffer
            0,                      // shift bit ignored
            0,                      // no accumulation buffer
            0, 0, 0, 0,             // accum bits ignored
            32,                     // 32-bit z-buffer
            0,                      // no stencil buffer
            0,                      // no auxiliary buffer
            PFD_MAIN_PLANE,         // main layer
            0,                      // reserved
            0, 0, 0                 // layer masks ignored
        };

        HDC hdc = ::GetDC(hwnd);

        auto pixel_format  =
        ::ChoosePixelFormat   (hdc, &pfd);
        ::DescribePixelFormat (hdc, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pfd );
        ::SetPixelFormat      (hdc, pixel_format, &pfd);

        handle =
        ::wglCreateContext    (hdc);
        ::wglMakeCurrent      (hdc, handle);
        ::ReleaseDC     (hwnd, hdc);

        win->native_handle2 = handle;
        break;
    }
    case WM_DESTROY:
    {
        ::wglMakeCurrent   (NULL, NULL);
        ::wglDeleteContext (handle);
        break;
    }
    case WM_SIZE:
    {
        HDC hdc = ::GetDC(hwnd);
        ::wglMakeCurrent (hdc, handle);
        ::ReleaseDC(hwnd, hdc);

        int w = GET_X_LPARAM(lparam);
        int h = GET_Y_LPARAM(lparam);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glViewport(0, 0, w, h);
        win->on_resize(xy(w,h));
        break;
    }
    case WM_PAINT:
    {
        //auto t0 = doc::ae::syntax::analysis::now();

        PAINTSTRUCT ps;
        HDC hdc = ::BeginPaint(hwnd, &ps);
        //int x = ps.rcPaint.left;
        //int y = ps.rcPaint.top;
        //int w = ps.rcPaint.right - x;
        //int h = ps.rcPaint.bottom - y;
        //::glScissor(x, y, w, h);
        //::glEnable(GL_SCISSOR_TEST);

        ::wglMakeCurrent (hdc, handle);

        win->renderr();

        ::glFinish();
        ::SwapBuffers(hdc);
        ::EndPaint(hwnd, &ps);

        //if (x != 2019 or y != 63) if (w != 1) {
        //auto t1 = doc::ae::syntax::analysis::now();
        //auto ms = doc::ae::syntax::analysis::ms(t1-t0);
        //doc::ae::syntax::analysis::events.debug("WM_PAINT "
        //+ std::to_string(x) + ","
        //+ std::to_string(y) + "-"
        //+ std::to_string(w) + ","
        //+ std::to_string(h) + " "
        //+ ms + " ms");
        //}
        return 0;
    }
    }

    return rc;
}

HWND MainWindowHwnd = nullptr;

void sys::window::create (str title)
{
    WNDCLASSW wc = {};
    wc.hInstance = ::GetModuleHandle(NULL);
    wc.lpszClassName = L"AE window class name";
    wc.lpfnWndProc = gpu ? GpuWindowProc : PixWindowProc;
    ::RegisterClassW(&wc);

    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;
    int w = CW_USEDEFAULT;
    int h = CW_USEDEFAULT;

    HWND handle = ::CreateWindowExW(
        WS_EX_APPWINDOW, // optional styles
        wc.lpszClassName, // window class
        winstr(title).c_str(),
        WS_OVERLAPPEDWINDOW,
        x, y, w, h,
        NULL, // parent window
        NULL, // menu
        ::GetModuleHandle(NULL),
        this // user data
        );

    if (!handle) throw std::system_error(
        ::GetLastError(), std::system_category(),
        "Failed to create window.");

    MainWindowHwnd = handle;

    ::ShowWindow(handle, SW_MAXIMIZE);
}
void sys::window::update()
{
    auto hwnd = (HWND)(native_handle1);
    for (xyxy r : image.updates)
    {
        RECT rect;
        rect.left   = r.l;
        rect.top    = r.t;
        rect.right  = r.r;
        rect.bottom = r.b;
        InvalidateRect(hwnd, &rect, FALSE);
        UpdateWindow  (hwnd);
    }
    image.updates.clear();
}
void sys::window::timing()
{
    ::PostMessage((HWND)(native_handle1), WM_COMMAND, 11111, 0);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR pCmdLine, int nCmdShow)
{
    ::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

    RECT r; ::SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);

    sys::screen::size.x = r.right - r.left;
    sys::screen::size.y = r.bottom - r.top;

    sys::app_instance app;

    MSG msg = {}; //...W for UNICODE
    while (::GetMessageW(&msg, NULL, 0, 0)) {
        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);
    }
}






