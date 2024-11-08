/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderFramework used by Game (Graphics API: DirectX12)

Update History: 2024/09/19 Create

Version : alpha_1.0.0

*/

#pragma once

#ifndef M_WINDOW
#define M_WINDOW

#include <Windows.h>
#include <string>

#include <Class-Def-Macro.h>
#include <Interfaces/IDisposable.h>
#include <Interfaces/IWindowInfo.h>

namespace MWindow
{
    class Window : public IWindowInfo , public MFramework::IDisposable
    {
        GENERATE_CLASS_NO_COPY(Window)
            
        public:
            bool InitWnd(UINT32 width, UINT32 height, const wchar_t* className, const wchar_t* wndTitle);
            void Dispose(void) noexcept override;
            HWND GetHWND(void) const;
            UINT32 GetWidth(void) const;
            UINT32 GetHeight(void) const;

        // スタティック関数
        private:
            static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);

        protected:
            virtual LRESULT CALLBACK WindowProcedure(UINT msg, WPARAM wparam, LPARAM lparam);

        private:
            HWND m_handleWindow;
            HINSTANCE m_hInstance;
            std::wstring m_className;
            bool m_isTerminated;

            UINT32 m_width;
            UINT32 m_height;

    };

// インライン定義

    // インライン宣言と定義は同じファイルで    
    inline HWND Window::GetHWND() const
    {
        return m_handleWindow;
    }   
    inline UINT32 Window::GetWidth() const
    {
        return m_width;
    }  
    inline UINT32 Window::GetHeight() const
    {
        return m_height;
    }  
}

#endif // M_WINDOW