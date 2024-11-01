/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderFramework used by Game (Graphics API: DirectX12)

Update History: 2024/09/19 Create

Version : alpha_1.0.0

*/

#pragma once

#ifndef M_WINDOW
#define M_WINDOW 1

#include <Windows.h>
#include <string>

#include <Interfaces/IDisposable.h>

class IWindowInfo
{
    public:
        virtual HWND GetHWND(void) const = 0;
        virtual UINT32 GetWidth(void) const = 0;
        virtual UINT32 GetHeight(void) const = 0;
};

namespace MWindow
{
    class Window : public IWindowInfo , public MFramework::IDisposable
    {
        public:
            Window();
            ~Window();
            
        public:
            bool InitWnd(UINT32 width, UINT32 height, const wchar_t* className, const wchar_t* wndTitle);
            void Dispose(void);
            HWND GetHWND(void) const;
            UINT32 GetWidth(void) const;
            UINT32 GetHeight(void) const;

        // コピー禁止    
        private:
            Window(const Window& other) = delete;
            Window operator=(const Window& other) = delete;

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