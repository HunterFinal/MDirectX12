/*

MRenderFramework
Author : MAI ZHICONG

Description : DirectX12 DXGIFactory Wrapper (Graphics API: DirectX12)

Update History: 2024/11/06 Create
           
Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_DX12_DXGIFACTORY
#define M_DX12_DXGIFACTORY

#include <ComPtr.h>
#include <Class-Def-Macro.h>
#include <Interfaces/IDisposable.h>

struct IDXGIFactory6;

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class DX12DXGIFactory final : public IDisposable
    {
      GENERATE_CLASS_NO_COPY(DX12DXGIFactory)

      public:
        /// @brief 
        /// 初期化する
        /// @param  
        void Init(void);

      public:
        void Dispose(void) noexcept override;

      public:
        IDXGIFactory6* GetFactory(void) const;
        operator IDXGIFactory6*() const noexcept;
        IDXGIFactory6* operator->() const noexcept;
      private:
        ComPtr<IDXGIFactory6> m_dxgiFactory;
    };
  }

  inline IDXGIFactory6* DX12DXGIFactory::GetFactory() const
  {
    return m_dxgiFactory.Get();
  }

  inline DX12DXGIFactory::operator IDXGIFactory6*() const noexcept
  {
    return m_dxgiFactory.Get();
  }

  inline IDXGIFactory6* DX12DXGIFactory::operator->() const noexcept
  {
    return m_dxgiFactory.Get();
  }
}

#endif
