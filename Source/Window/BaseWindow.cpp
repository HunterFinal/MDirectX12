/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderFramework used by Game (Graphics API: DirectX12)

Update History: 2024/09/19 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <Window/BaseWindow.h>

#include <tchar.h> // use of _T macro

namespace MWindow
{
    // TODO temp
    Window::Window()
    {
        m_window = {};

        m_window.cbSize = sizeof(WNDCLASSEX);
        m_window.lpfnWndProc = (WNDPROC)MessageRouter;
        m_window.lpszClassName = _T("MRenderFramework");
        m_window.hInstance = GetModuleHandle(nullptr);

        RegisterClassEx(&m_window);

        RECT windowRect = {0,0,1920,1080};

        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

        m_handle = CreateWindow(
                                    m_window.lpszClassName,             // クラス名指定
                                    _T("MRenderFramworkテスト"),        // タイトルバー文字
                                    WS_OVERLAPPEDWINDOW,                // タイトルバーと境界線があるウィンドウ　
                                    CW_USEDEFAULT,                      // 表示x座標はOSにお任せ
                                    CW_USEDEFAULT,                      // 表示y座標はOSにお任せ
                                    windowRect.right - windowRect.left, // ウィンドウ幅
                                    windowRect.bottom - windowRect.top, // ウィンドウ高
                                    nullptr,                            // 親ウインドウハンドル
                                    nullptr,                            // メニューハンドル
                                    m_window.hInstance,                 // 呼び出しアプリケーションハンドル
                                    nullptr                             // 追加パラメーター
                                                                        );
        
        // ウインドウ表示
        ShowWindow(m_handle,SW_SHOW);
    }

    Window::~Window()
    {
        UnregisterClass(m_window.lpszClassName, m_window.hInstance);
    }
    
    HWND Window::GetHandle()
    {
        return m_handle;
    }   
    LRESULT Window::MessageRouter(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        // ウィンドウが破棄されたら呼ばれる
        if (msg == WM_DESTROY)
        {
            // OSに対して「アプリは終わる」と伝える
            PostQuitMessage(0);

            return 0;
        }
        return DefWindowProc(hwnd, msg, wparam, lparam);

    }

    LRESULT Window::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        return LRESULT();
    }
}
