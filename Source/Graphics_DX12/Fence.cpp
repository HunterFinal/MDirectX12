/*

MRenderFramework
Author : MAI ZHICONG

Description : D3D12 Fence Wrapper (Graphics API: DirectX12)

Update History: 2024/09/19 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <Graphics_DX12/Fence.h>

#include <d3d12.h>
#include <cassert>

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    Fence::Fence()
      : m_fence(nullptr)
      , m_event(nullptr)
      , m_fenceCount(0)
      , m_isInitialized(false)
    { }

    Fence::~Fence()
    {
      Dispose();
    }

    void Fence::Init(ID3D12Device* device)
    {
      assert(device != nullptr);

      // TODO CreateEventEx
      m_event = CreateEvent(nullptr, false, false, nullptr);

      assert(m_event != nullptr);

      HRESULT result = device->CreateFence(  
                                          m_fenceCount, 
                                          D3D12_FENCE_FLAG_NONE, 
                                          IID_PPV_ARGS(m_fence.ReleaseAndGetAddressOf())
                                        );
      assert(SUCCEEDED(result));
      m_isInitialized = true;
    }

    void Fence::Wait(ID3D12CommandQueue* commandQueue, UINT32 waitTime)
    {
      if (commandQueue == nullptr)
      {
          return;
      }

      if (!m_isInitialized)
      {
          return;
      }

      commandQueue->Signal(m_fence.Get(), ++m_fenceCount);

      if(m_fence->GetCompletedValue() != m_fenceCount)
      {
          m_fence->SetEventOnCompletion(m_fenceCount, m_event);

          // イベントが発生するまで待ち続ける
          WaitForSingleObject(m_event, waitTime);

      }
    }

    void Fence::Dispose() noexcept
    {
      if (m_event != nullptr)
      {
        CloseHandle(m_event);
        m_event = nullptr;
      }

      m_fence.Reset();
      m_fenceCount = 0;
      m_isInitialized = false;
    }
  }
}