/*

MRenderFramework
Author : MAI ZHICONG

Description : DirectX12 SwapChain Wrapper (Graphics API: DirectX12)

Update History: 2024/11/07 Create
           
Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include<Graphics_DX12/DX12SwapChain.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <Interfaces/IWindowInfo.h>
#include <cassert>

namespace MFramework
{
  SwapChain::SwapChain()
    :m_swapChain(nullptr)
  {

  }

  SwapChain::~SwapChain()
  {
    Dispose();
  }

  void SwapChain::Init(IDXGIFactory6* dxgiFactory, ID3D12CommandQueue* commandQueue, HWND hWnd, size_t frameCount)
  {
    if (dxgiFactory == nullptr || commandQueue == nullptr || hWnd == nullptr)
    {
      return;
    }

    RECT windowRect;
    GetClientRect(hWnd, &windowRect);


    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

    swapChainDesc.Width = windowRect.right - windowRect.left;
    swapChainDesc.Height = windowRect.bottom - windowRect.top;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Stereo = false;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
    swapChainDesc.BufferCount = static_cast<UINT>(frameCount);
    // バックバッファーは伸び縮み可能
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    // フリップ後は速やかに破棄
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    // 特に指定なし
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    // ウインドウ⇔フルスクリーン切り替え可能
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    HRESULT result = S_OK;

    // TODO 大事
    // 本来はQueryInterfaceなどを用いて IDXGISwapChain4*への変換チェックをするが、わかりやすさ重視のためキャストで対応
    ComPtr<IDXGISwapChain1> tmpSwapChain;
    result = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hWnd, &swapChainDesc, nullptr, nullptr, tmpSwapChain.ReleaseAndGetAddressOf());

    assert(SUCCEEDED(result));

    result = tmpSwapChain.As(&m_swapChain);

    assert(SUCCEEDED(result));
  }

  void SwapChain::Dispose(void) noexcept
  {
    m_swapChain.Reset();
  }
}