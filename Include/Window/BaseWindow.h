/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderFramework used by Game (Graphics API: DirectX12)

Update History: 2024/09/19 Create

Version : alpha_1.0.0

*/

#pragma once

#ifndef _M_WINDOW
#define _M_WINDOW

#include <Windows.h>
#include <string>

namespace MWindow
{
    class Window
    {
        public:
            Window();
            ~Window();
            
        public:
            bool InitWnd(UINT32 width, UINT32 height, const wchar_t* className, const wchar_t* wndTitle);
            void Term(void);
            HWND GetHWND(void) const;
            
        private:
            Window(const Window& other) = delete;
            Window operator=(const Window& other) = delete;

        private:
            static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);

        protected:
            virtual LRESULT CALLBACK WindowProcedure(UINT msg, WPARAM wparam, LPARAM lparam);

        private:
            HWND m_handle;
            HINSTANCE m_hInstance;
            std::wstring m_className;
            bool m_isTerminated;

    };

}

// インライン定義
namespace MWindow
{

    // インライン宣言と定義は同じファイルで    
    inline HWND Window::GetHWND() const
    {
        return m_handle;
    }   
}

#endif // _M_WINDOW