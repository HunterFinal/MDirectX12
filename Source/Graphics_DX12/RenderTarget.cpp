/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderTarget Wrapper (Graphics API: DirectX12)

Update History: 2024/11/12 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <Graphics_DX12/RenderTarget.h>
#include <Graphics_DX12/DescriptorHandle.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <cassert>

namespace MFramework
{
  RenderTarget::RenderTarget()
    : m_renderTarget(nullptr)
  {

  }

  RenderTarget::~RenderTarget()
  {
    Dispose();
  }

  RenderTarget::RenderTarget(RenderTarget&& other)
  {
    m_renderTarget = other.m_renderTarget;
    other.m_renderTarget = nullptr;
  }

  RenderTarget& RenderTarget::operator=(RenderTarget&& other) &
  {
    m_renderTarget = other.m_renderTarget;
    other.m_renderTarget = nullptr;

    return *this;
  }

  void RenderTarget::Create(ID3D12Device* device, IDXGISwapChain4* swapChain, size_t index, DescriptorHandle* descHandle)
  {
    if (device == nullptr || swapChain == nullptr || descHandle == nullptr)
    {
      return;
    }

    D3D12_RENDER_TARGET_VIEW_DESC desc = {};

    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapChain->GetDesc(&swapDesc);
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;          
    desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipSlice = 0;
    desc.Texture2D.PlaneSlice = 0;

    HRESULT result = S_OK;
    result = swapChain->GetBuffer(static_cast<UINT>(index), IID_PPV_ARGS(m_renderTarget.ReleaseAndGetAddressOf()));

    assert(SUCCEEDED(result));

    if (descHandle->HasCPUHandle())
    {
      device->CreateRenderTargetView(m_renderTarget.Get(), &desc, descHandle->CPUHandle);
    }
  }

  void RenderTarget::Dispose() noexcept
  {
    m_renderTarget.Reset();
  }
}