/*

MRenderFramework
Author : MAI ZHICONG

Description : DirectX12 SwapChain Wrapper (Graphics API: DirectX12)

Update History: 2024/11/07 Create
           
Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_DX12_SWAPCHAIN
#define M_DX12_SWAPCHAIN

#include <ComPtr.h>
#include <Class-Def-Macro.h>
#include <Interfaces/IDisposable.h>

struct IDXGIFactory6;
struct ID3D12CommandQueue;
struct IDXGISwapChain4;

namespace MWindow
{
  class IWindowInfo;
}

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class SwapChain final : public IDisposable
    {
      GENERATE_CLASS_NO_COPY(SwapChain)

      public:
        void Init(IDXGIFactory6*, ID3D12CommandQueue*, MWindow::IWindowInfo*, int);

      public:
        void Dispose(void) noexcept override;

      public:
        IDXGISwapChain4* Get(void) const;
        IDXGISwapChain4* operator->() const noexcept;

      private:
        ComPtr<IDXGISwapChain4> m_swapChain;
    };  
  }

  inline IDXGISwapChain4* SwapChain::Get(void) const
  {
    return m_swapChain.Get();
  }

  inline IDXGISwapChain4* SwapChain::operator->() const noexcept
  {
    return m_swapChain.Get();
  }
}

#endif