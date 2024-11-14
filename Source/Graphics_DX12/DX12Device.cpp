/*

MRenderFramework
Author : MAI ZHICONG

Description : DirectX12 Device Wrapper (Graphics API: DirectX12)

Update History: 2024/11/01 Create
           
Version : alpha_1.0.0

Encoding : UTF-8 

*/
#include <Graphics_DX12/DX12Device.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <vector>
#include <string>

#include <cassert>

namespace
{
  enum class GPULevel : INT8
  {
      NVIDIA      = 5,
      Amd         = 4,
      Intel       = 3,
      Arm         = 2,
      Qualcomm    = 1,

      Lowest      = -128,
  };
  
  D3D_FEATURE_LEVEL levels[] = 
  {
    D3D_FEATURE_LEVEL_12_2,
    D3D_FEATURE_LEVEL_12_1,
    D3D_FEATURE_LEVEL_12_0,
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
  };

  void GetAdapter(IDXGIFactory6* dxgiFactory, IDXGIAdapter** adapter)
  {
    // 利用可能なアタブターの列挙用
    std::vector<IDXGIAdapter*> adapters;
    adapters.reserve(5);

    IDXGIAdapter* tmpAdapter = nullptr;

    int adapterIndex = 0;
    // アタブターを探す
    while (dxgiFactory->EnumAdapters(adapterIndex, &tmpAdapter) != DXGI_ERROR_NOT_FOUND)
    {
      adapters.emplace_back(tmpAdapter);
      ++adapterIndex;
    }

    tmpAdapter = nullptr;

    GPULevel level = GPULevel::Lowest;
    // アダプターを識別するための情報を取得
    for(const auto& adpt : adapters)
    {
      DXGI_ADAPTER_DESC adesc = {};
      // アダプターの説明オブジェクト取得
      adpt->GetDesc(&adesc);

      std::wstring strDesc(adesc.Description);
      // 探したいアダプターの名前を確認
      if (strDesc.find(L"NVIDIA") != std::wstring::npos)
      {
        if (level < GPULevel::NVIDIA)
        {
          level = GPULevel::NVIDIA;
          *adapter = adpt;
        }
      }
      else if (strDesc.find(L"Amd") != std::wstring::npos)
      {
        if (level < GPULevel::Amd)
        {
          level = GPULevel::Amd;
          *adapter = adpt;
        }
      }
      else if (strDesc.find(L"Intel") != std::wstring::npos)
      {
        if (level < GPULevel::Intel)
        {
          level = GPULevel::Intel;
          *adapter = adpt;
        }
      }
      else if (strDesc.find(L"Arm") != std::wstring::npos)
      {
        if (level < GPULevel::Arm)
        {
          level = GPULevel::Arm;
          *adapter = adpt;
        }
      }
      else if (strDesc.find(L"Qualcomm") != std::wstring::npos)
      {
        if (level < GPULevel::Qualcomm)
        {
          level = GPULevel::Qualcomm;
          *adapter = adpt;
        }
      }
    }
  }
}

namespace MFramework
{
  DX12Device::DX12Device()
    : m_device(nullptr)
  { }
  
  DX12Device::~DX12Device()
  {
    Dispose();
  }

  void DX12Device::Init(IDXGIFactory6* dxgiFactory)
  {
    if (dxgiFactory == nullptr)
    {
      return;
    }
   
    ComPtr<IDXGIAdapter> foundAdapter = nullptr;
    GetAdapter(dxgiFactory, foundAdapter.ReleaseAndGetAddressOf());

    for (auto lv : levels)
    {
      if (SUCCEEDED(D3D12CreateDevice(foundAdapter.Get(), lv, IID_PPV_ARGS(m_device.ReleaseAndGetAddressOf()))))
      {
        break;
      }
    }

    assert(m_device.Get() != nullptr);

    m_device->SetName(L"MyDevice");
  }

  void DX12Device::Dispose() noexcept
  { 
    m_device.Reset();
  }
}