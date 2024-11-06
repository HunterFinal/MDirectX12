/*

MRenderFramework
Author : MAI ZHICONG

Description : DirectX12 Device Wrapper (Graphics API: DirectX12)

Update History: 2024/11/01 Create
           
Version : alpha_1.0.0

Encoding : UTF-8 

*/
#include <Graphics_DX12/DX12DXGIFactory.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#include <cassert>

namespace MFramework
{
  DX12DXGIFactory::DX12DXGIFactory()
    : m_dxgiFactory(nullptr)
  { }
  
  DX12DXGIFactory::~DX12DXGIFactory()
  {
    Dispose();
  }

  void DX12DXGIFactory::Init()
  {
    if (m_dxgiFactory.Get() != nullptr)
    {
      return;
    }

    HRESULT result = S_OK;
#ifdef _DEBUG                           
    result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(m_dxgiFactory.GetAddressOf()));
#else
    result = CreateDXGIFactory1(IID_PPV_ARGS(_dxgiFactory.GetAddressOf()));
#endif 

    assert(SUCCEEDED(result));
  }

  void DX12DXGIFactory::Dispose() noexcept
  {
    m_dxgiFactory.Reset();
  }
}