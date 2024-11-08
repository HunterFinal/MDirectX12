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

#pragma once

#ifndef M_DX12_FENCE
#define M_DX12_FENCE

struct ID3D12Device;
struct ID3D12CommandQueue;
struct ID3D12Fence;

#include <ComPtr.h>

namespace MFramework
{
    inline namespace MGraphics_DX12
    {
        class Fence final
        {
            private:
                Fence(const Fence&) = delete;
                Fence& operator=(const Fence&) = delete;

            public:
                Fence();
                ~Fence();

            public:
                bool Init(ID3D12Device*);

                void Wait(ID3D12CommandQueue*, UINT32 = INFINITE);

            private:
                void Terminate() noexcept;

            private:
                ComPtr<ID3D12Fence> m_fence;
                HANDLE m_event;
                UINT64 m_fenceCount;
                bool m_isDisposed : 1;
                bool m_isInitialized : 1;
        };
    }
}
#endif