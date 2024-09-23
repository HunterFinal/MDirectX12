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

namespace MWindow
{
    class Window
    {
        public:
            Window();
            ~Window();
            
        public:
            HWND GetHandle();
            
        private:
            Window(const Window& other) = delete;
            Window operator=(const Window& other) = delete;

        private:
            static LRESULT CALLBACK MessageRouter(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        protected:
            virtual LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
        
        private:
            WNDCLASSEX m_window;
            HWND m_handle;
    };
}

#endif // _M_WINDOW