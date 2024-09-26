/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderFramework used by Game (Graphics API: DirectX12)

Update History: 2024/09/19 Create
                2024/09/26 Update constructor
                           Create virtual WndProc

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <Window/BaseWindow.h>

#include <tchar.h> // use of _T macro

#include <sstream>
#include <DebugHelper>

namespace
{
    static POINT pt {0,0}; 
    wchar_t message[256] = L"\0";
    static UINT64 DEFAULT_CLASS_NAME_CNT = 42;
    std::wstring GetDefaultClassName()
    {
        std::wstringstream defaultClassName(L"Default_Class_Name");
        defaultClassName << std::to_wstring(DEFAULT_CLASS_NAME_CNT++);
        return defaultClassName.str();
    }
}
namespace MWindow
{
    // TODO temp
    Window::Window()
        : m_handle(nullptr)
        , m_hInstance(nullptr)
        , m_className()
        , m_isTerminated(false)
    {
        
    }

    Window::~Window()
    {
        if (!m_isTerminated)
        {
            Term();      
        }
    }

    bool Window::InitWnd(UINT32 width, UINT32 height, const wchar_t* className, const wchar_t* wndTitle)
    {
        m_hInstance = GetModuleHandle(nullptr);
        {
            if (m_hInstance == nullptr)
            {
                return false;
            }
        }

        // ClassName
        // ???
        // nullチェック
        if (className != nullptr)
        {
            m_className = className;
        }
        // nullだったらデフォルトの名前を入れる
        else
        {
            m_className = ::GetDefaultClassName();
        }

        // ウインドウタイトル
        // nullだったらデフォルトの名前を入れる
        std::wstring wndTitleStr;
        if(wndTitle != nullptr)
        {
            wndTitleStr = wndTitle;
        }

        WNDCLASSEX wc = {};

        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;                     // ウィンドウのサイズを変更したらウインドウを更新
        wc.lpfnWndProc = MessageRouter;
        wc.hInstance = m_hInstance;
        wc.hIcon = LoadIcon(m_hInstance, IDI_APPLICATION);
        wc.hCursor = LoadCursor(m_hInstance, IDC_ARROW);
        wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = m_className.c_str();
        wc.hIconSm = LoadIcon(m_hInstance, IDI_APPLICATION);

        RegisterClassEx(&wc);

        RECT windowRect = {};

        windowRect.left = 0;
        windowRect.top = 0;
        windowRect.right = static_cast<LONG>(width);
        windowRect.bottom = static_cast<LONG>(height);

        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

        m_handle = CreateWindowEx(
                                    0,
                                    wc.lpszClassName,                   // クラス名指定
                                    wndTitleStr.c_str(),                // タイトルバー文字
                                    WS_OVERLAPPEDWINDOW,                // タイトルバーと境界線があるウィンドウ　
                                    CW_USEDEFAULT,                      // 表示x座標はOSにお任せ
                                    CW_USEDEFAULT,                      // 表示y座標はOSにお任せ
                                    windowRect.right - windowRect.left, // ウィンドウ幅
                                    windowRect.bottom - windowRect.top, // ウィンドウ高
                                    nullptr,                            // 親ウインドウハンドル
                                    nullptr,                            // メニューハンドル
                                    m_hInstance,                        // 呼び出しアプリケーションハンドル
                                    this                                // 追加パラメーター(後でユーザー定義のWndProcで使う)
                                  );
        if(m_handle == nullptr)
        {
            return false;
        }

        // ウインドウ表示
        ShowWindow(m_handle,SW_SHOW);
        UpdateWindow(m_handle);
        SetFocus(m_handle);

        return true;
    }

    void Window::Term()
    {
        if (m_hInstance != nullptr)
        {
            UnregisterClass(m_className.c_str(), m_hInstance);
            m_handle = nullptr;
            m_hInstance = nullptr;

            m_isTerminated = true;
        }

    }

    LRESULT Window::WindowProcedure(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch (msg)
        {
        case WM_KEYDOWN:
        {
            switch (wparam)
            {
            case VK_ESCAPE:
            {
                PostMessage(m_handle, WM_CLOSE, 0, 0);
            }
            }
        }
        break;
        case WM_MOUSEMOVE:
        {
            _stprintf_s(message, _T("現在の座標（%d,%d) \n"), LOWORD(lparam), HIWORD(lparam));
            //再描画メッセージを発生させる
            Debug.Log("%ls", message);
            ::OutputDebugStringW(message);
        }
        break;
        // ウィンドウが破棄されたら呼ばれる
        case WM_DESTROY:
        {
            // OSに対して「アプリは終わる」と伝える
            PostQuitMessage(0);
        }
        break;
        default:
        {
            return DefWindowProc(m_handle, msg, wparam, lparam);
        }
        break;
        }

        return 0;
    }
    LRESULT Window::MessageRouter(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
    { 
        Window* This = nullptr;
        if (msg == WM_CREATE)
        {
            LPCREATESTRUCT create = reinterpret_cast<LPCREATESTRUCT>(lparam);
            This = reinterpret_cast<Window*>(create->lpCreateParams);
            This->m_handle = hWnd;
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)This);

        }
        else
        {
            This = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }

        if(This != nullptr)
        {
            return This->WindowProcedure(msg, wparam, lparam);
        }
        else
        {
            return DefWindowProc(hWnd, msg, wparam, lparam);
        }
    }
}
