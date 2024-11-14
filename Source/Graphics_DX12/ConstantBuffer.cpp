/*

MRenderFramework
Author : MAI ZHICONG

Description : Constant Buffer (Graphics API: DirectX12)

Update History: 2024/11/10 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <Graphics_DX12/ConstantBuffer.h>
#include <Graphics_DX12/DescriptorHandle.h>

#include <cassert>

namespace
{
  // 0xff
  constexpr UINT8 ALIGNMENT = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1; 

  UINT64 getConstantBufferAlignment(size_t size)
  {
    return static_cast<UINT64>((size + ALIGNMENT) & ~ALIGNMENT);
  }
}

namespace MFramework
{
  ConstantBuffer::ConstantBuffer()
    : m_constantBuffer(nullptr)
    , m_viewDesc()
    , m_descHandle()
    , m_mappedData(nullptr)
    , m_isMapped(false)
  {
    memset(&m_viewDesc, 0, sizeof(m_viewDesc));
  }

  ConstantBuffer::~ConstantBuffer()
  {
    Dispose();
  }

  bool ConstantBuffer::Create(
                                ID3D12Device* device,
                                const DescriptorHandle& handle,
                                size_t size,
                                const void* srcData
                              )
  {
    if (device == nullptr || size == 0)
    {
      return false;
    }

    if (m_constantBuffer.Get() != nullptr)
    {
      return false;
    }

    m_descHandle.CPUHandle = handle.CPUHandle;
    m_descHandle.GPUHandle = handle.GPUHandle;

    D3D12_HEAP_PROPERTIES heapProp = {};

    heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;                       // CPUからアクセスできる（Mapできる）※DEFAULTより遅い
    heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;   // TypeはCUSTOMじゃないためUNKNOWNでよい
    heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;    // TypeはCUSTOMじゃないためUNKNOWNでよい
    heapProp.CreationNodeMask = 1;  // 0 と同じ意味
    heapProp.VisibleNodeMask = 1;

    auto alignedSize = getConstantBufferAlignment(size);
    // 定数バッファー情報
    D3D12_RESOURCE_DESC resourceDesc = {};

    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;                   // バッファーに使うためBUFFERを指定
    resourceDesc.Alignment = 0;                                                 // ??
    resourceDesc.Width = alignedSize;                                           // サイズ
    resourceDesc.Height = 1;                                                    // 幅で表現しているので１とする
    resourceDesc.DepthOrArraySize = 1;                                          // 1でよい
    resourceDesc.MipLevels = 1;                                                 // 1でよい
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;                                  // 今回は画像ではないためUNKNOWNでよい
    resourceDesc.SampleDesc.Count = 1;                                          // アンチエイリアシングを行うときのパラメーター。※ 0だとデータがないことになってしまうため、1にする
    resourceDesc.SampleDesc.Quality = 0;                                        // ??
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;                       // UNKNOWNと指定すると、自動で最適なレイアウトに設定しようとする　※今回はテクスチャではないため不適切です
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;                              // NONEでよい※要調べ

    HRESULT result = S_OK;

    result = device->CreateCommittedResource(
                                              &heapProp,
                                              D3D12_HEAP_FLAG_NONE,
                                              &resourceDesc,
                                              D3D12_RESOURCE_STATE_GENERIC_READ,
                                              nullptr,
                                              IID_PPV_ARGS(m_constantBuffer.ReleaseAndGetAddressOf())
                                            );

    assert(SUCCEEDED(result));
    
    // マップする
    if (srcData != nullptr)
    {
      map(alignedSize, srcData);
    }

    if (!m_descHandle.HasCPUHandle())
    {
      return false;
    }

    m_viewDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
    m_viewDesc.SizeInBytes = static_cast<UINT>(alignedSize);
    device->CreateConstantBufferView(&m_viewDesc, m_descHandle.CPUHandle);
    
    return true;
  }

  void ConstantBuffer::Remap(size_t size, const void* srcData)
  {
    if (m_constantBuffer.Get() == nullptr)
    {
      return;
    }

    if (srcData == nullptr)
    {
      return;
    }

    map(size, srcData);
  }

  void ConstantBuffer::Dispose(void) noexcept
  {
    if (m_constantBuffer.Get() != nullptr)
    {
      unmap();
    }
    m_constantBuffer.Reset();
  }

  void ConstantBuffer::map(size_t size, const void* srcData)
  {
    auto alignedSize = getConstantBufferAlignment(size);
    if (m_constantBuffer->GetDesc().Width != alignedSize)
    {
      return;
    }
    
    if (!m_isMapped)
    {
      HRESULT result = m_constantBuffer->Map(0, nullptr, &m_mappedData);
      m_isMapped = true;
    }

    memcpy_s(m_mappedData, alignedSize, srcData, alignedSize);
  }

  void ConstantBuffer::unmap()
  {
    m_constantBuffer->Unmap(0, nullptr);
    m_mappedData = nullptr;
    m_isMapped = false;
  }
}