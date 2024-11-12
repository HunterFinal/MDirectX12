/*

MRenderFramework
Author : MAI ZHICONG

Description : DescriptorHeap Wrapper (Graphics API: DirectX12)

Update History: 2024/11/12 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <Graphics_DX12/DescriptorHeap.h>

#include <cassert>

namespace MFramework
{
  DescriptorHeap::DescriptorHeap()
    : m_descHeap(nullptr)
    , m_heapType(D3D12DescHeapType::None)
    , m_numDesc(0)
    , m_incrementSize(0)
  {}

  DescriptorHeap::~DescriptorHeap()
  {
    Dispose();
  }

  void DescriptorHeap::Init(ID3D12Device* device, D3D12DescHeapType heapType, size_t numDesc)
  {
    if (device == nullptr || numDesc == 0)
    {
      return;
    }

    m_heapType = heapType;
    m_numDesc = numDesc;

    D3D12_DESCRIPTOR_HEAP_DESC desc = {};

    switch (m_heapType)
    {
      // テクスチャバッファー（SRV）や定数バッファー（CBV）であれば D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
      case D3D12DescHeapType::CBV_SRV_UAV :
      {
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
      }
      break;
      case D3D12DescHeapType::RTV:
      {
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
      }
      break;
      case D3D12DescHeapType::SAMPLER:
      case D3D12DescHeapType::DSV:
      case D3D12DescHeapType::NUM_TYPES:
      default:
      {
        #ifdef _DEBUG
          ::OutputDebugStringA("Not Implemented");
        #endif

        assert(false);
      }
      break;
    }

    m_incrementSize = device->GetDescriptorHandleIncrementSize(desc.Type);
    // 単一GPU
    desc.NodeMask = 0;
    desc.NumDescriptors = static_cast<UINT>(m_numDesc);

    HRESULT result = S_OK;
    result = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_descHeap.ReleaseAndGetAddressOf()));

    assert(SUCCEEDED(result));
  }

  DescriptorHandle DescriptorHeap::GetHandle(size_t index) const
  {
    if (m_descHeap.Get() == nullptr || index >= m_numDesc)
    {
      return DescriptorHandle();
    }

    DescriptorHandle handle;
    handle.CPUHandle = m_descHeap->GetCPUDescriptorHandleForHeapStart();
    handle.CPUHandle.ptr += index * m_incrementSize;
    handle.GPUHandle = m_descHeap->GetGPUDescriptorHandleForHeapStart();
    handle.GPUHandle.ptr += index * m_incrementSize;

    return handle;
  }

  void DescriptorHeap::Dispose() noexcept
  {
    m_descHeap.Reset();
    m_heapType = D3D12DescHeapType::None;
    m_numDesc = 0;
    m_incrementSize = 0;
  }

}