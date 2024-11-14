/*

MRenderFramework
Author : MAI ZHICONG

Description : D3D12 Fence Wrapper (Graphics API: DirectX12)

Update History: 2024/09/19 Create

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
#include <Class-Def-Macro.h>
#include <Interfaces/IDisposable.h>

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class Fence final : public IDisposable
    {
      GENERATE_CLASS_NO_COPY(Fence)

      public:
        void Init(ID3D12Device*);
        void Wait(ID3D12CommandQueue*, UINT32 = INFINITE);

      public:
        void Dispose(void) noexcept override;

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