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
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

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
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };
}

namespace MFramework
{
  DX12Device::DX12Device()
    : m_device(nullptr)
  { }
  
  DX12Device::~DX12Device()
  {
    m_device.Reset();
  }

  void DX12Device::Init(IDXGIFactory6* _dxgiFactory)
  {
    if (m_device.Get() != nullptr)
    {
      return;
    }

    assert(_dxgiFactory != nullptr);
   
    // 利用可能なアタブターの列挙用
    std::vector<IDXGIAdapter*> adapters;

    IDXGIAdapter* tmpAdapter = nullptr;

    int adapterIndex = 0;
    // アタブターを探す
    while(_dxgiFactory->EnumAdapters(adapterIndex, &tmpAdapter) != DXGI_ERROR_NOT_FOUND)
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
          tmpAdapter = adpt;
        }
      }
      else if (strDesc.find(L"Amd") != std::wstring::npos)
      {
        if (level < GPULevel::Amd)
        {
          level = GPULevel::Amd;
          tmpAdapter = adpt;
        }
      }
      else if (strDesc.find(L"Intel") != std::wstring::npos)
      {
        if (level < GPULevel::Intel)
        {
          level = GPULevel::Intel;
          tmpAdapter = adpt;
        }
      }
      else if (strDesc.find(L"Arm") != std::wstring::npos)
      {
        if (level < GPULevel::Arm)
        {
          level = GPULevel::Arm;
          tmpAdapter = adpt;
        }
      }
      else if (strDesc.find(L"Qualcomm") != std::wstring::npos)
      {
        if (level < GPULevel::Qualcomm)
        {
          level = GPULevel::Qualcomm;
          tmpAdapter = adpt;
        }
      }
    }

    for(auto lv : levels)
    {
      if(D3D12CreateDevice(tmpAdapter, lv, IID_PPV_ARGS(m_device.GetAddressOf())) == S_OK)
      {
        break;
      }
    }

    assert(m_device.Get() != nullptr);
  }
}