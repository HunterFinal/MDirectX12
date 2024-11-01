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

#ifndef M_DISPOSE
#define M_DISPOSE 1

namespace MFramework
{
    class IDisposable
    {
        public:
            virtual void Dispose(void) = 0;
            virtual ~IDisposable() {}
    };
}

#endif


