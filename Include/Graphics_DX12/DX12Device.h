/*

MRenderFramework
Author : MAI ZHICONG

Description : DirectX12 Device Wrapper (Graphics API: DirectX12)

Update History: 2024/11/01 Create
           
Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_DX12_DEVICE
#define M_DX12_DEVICE

#include "GraphicsClassBaseInclude.h"

struct ID3D12Device;
struct IDXGIFactory6;

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class DX12Device final : public IDisposable
    {
      GENERATE_CLASS_NO_COPY(DX12Device)

      public:
        /// @brief 
        /// 初期化する
        /// @param  
        void Init(IDXGIFactory6*);

      public:
        void Dispose(void) noexcept override;

      public:
        ID3D12Device* Get(void) const;
        ID3D12Device* operator->() const noexcept;

      private:
        ComPtr<ID3D12Device> m_device;
    };
  }
  // MGraphics_DX12

  inline ID3D12Device* DX12Device::Get() const
  {
    return m_device.Get();
  }

  inline ID3D12Device* DX12Device::operator->() const noexcept
  {
    return m_device.Get();
  }
}
// MFramework

#endif
// M_DX12_DEVICE
