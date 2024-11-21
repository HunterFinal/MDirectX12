/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderTarget Wrapper (Graphics API: DirectX12)

Update History: 2024/11/12 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_DX12_RENDERTARGET
#define M_DX12_RENDERTARGET

#include "GraphicsClassBaseInclude.h"

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class DescriptorHandle;
  }
}

struct ID3D12Device;
struct ID3D12Resource;
struct IDXGISwapChain4;

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class RenderTarget final : public IDisposable
    {
      GENERATE_CLASS_NO_COPY(RenderTarget)

      public:
        void Create(ID3D12Device*, IDXGISwapChain4*, size_t index, DescriptorHandle*);
        ID3D12Resource* Get() const;

      public:
        void Dispose(void) noexcept override;

      private:
        ComPtr<ID3D12Resource> m_renderTarget;
    };

    inline ID3D12Resource* RenderTarget::Get() const
    {
      return m_renderTarget.Get();
    }
  }
}

#endif