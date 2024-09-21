/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderFramework used by Game (Graphics API: DirectX12)

Update History: 2024/09/19 Create

Version : alpha_1.0.0

*/

#pragma once

namespace MDebug
{
    class Debug
    {
        public:
            void Log(const char* format,...) const;
    };
}