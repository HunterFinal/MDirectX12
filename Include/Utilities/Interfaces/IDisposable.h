/*

MRenderFramework
Author : MAI ZHICONG

Description : Disposable interface (Graphics API: DirectX12)

Update History: 2024/09/19 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#ifndef M_DISPOSE
#define M_DISPOSE

namespace MFramework
{
    class IDisposable
    {
        public:
            virtual void Dispose(void) noexcept = 0;
            virtual ~IDisposable() {}
    };
}

#endif


