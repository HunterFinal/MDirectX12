/*

MRenderFramework
Author : MAI ZHICONG

Description : DirectX12 Device Wrapper (Graphics API: DirectX12)

Update History: 2024/11/01 Create
           
Version : alpha_1.0.0

Encoding : UTF-8 

*/

#ifndef M_DX12_DEVICE
#define M_DX12_DEVICE

#include <ComPtr.h>
#include <Class-Def-Macro.h>

struct ID3D12Device;
struct IDXGIFactory6;

namespace MFramework
{
  class DX12Device final
  {
    GENERATE_CLASS_NO_COPY(DX12Device)

    public:
      void Init(IDXGIFactory6*);

    public:
      ID3D12Device* GetDevice();
      operator ID3D12Device*() const noexcept;
      ID3D12Device* operator->() const noexcept;
    private:
      ComPtr<ID3D12Device> m_device;
  };

  inline ID3D12Device* DX12Device::GetDevice()
  {
    return m_device.Get();
  }

  inline DX12Device::operator ID3D12Device*() const noexcept
  {
    return m_device.Get();
  }

  inline ID3D12Device* DX12Device::operator->() const noexcept
  {
    return m_device.Get();
  }
}

#endif
