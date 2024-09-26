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

#ifndef _M_FENCE
#define _M_FENCE

#include <d3d12.h>

#include <wrl.h>

// TODO
class ID3D12DeviceNeededInit
{
    public:
        virtual bool Init(ID3D12Device* device) = 0;
};

namespace MFramework
{
    namespace MGraphics_DX12
    {
        class Fence : public ID3D12DeviceNeededInit
        {
            private:
                Fence(const Fence&) = delete;
                Fence& operator=(const Fence&) = delete;

            public:
                Fence();
                ~Fence();

            public:
                bool Init(ID3D12Device* device);

                void WaitSingle(ID3D12CommandQueue* commandQueue);

            private:
                void Terminate() noexcept;

            private:
                Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
                HANDLE m_event;
                UINT64 m_fenceCount;

            private:
                bool m_isInitialized;
        };
    }
}
#endif