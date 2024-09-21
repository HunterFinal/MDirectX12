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
        private:
            Window(const Window& other) = delete;
            Window operator=(const Window& other) = delete;
        
        private:
            WNDCLASSEX window;
    };
}

#endif