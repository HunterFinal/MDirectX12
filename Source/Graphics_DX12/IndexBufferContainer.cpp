/*

MRenderFramework
Author : MAI ZHICONG

Description : IndexBuffer Container (Graphics API: DirectX12)

Update History: 2024/11/08 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <Graphics_DX12/IndexBufferContainer.h>

#include <cassert>

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    IndexBufferContainer::IndexBufferContainer()
      : m_idxBuffer(nullptr)
      , m_idxBufferView()
    { 
      memset(&m_idxBufferView, 0, sizeof(m_idxBufferView));
    }

    IndexBufferContainer::~IndexBufferContainer()
    {
      Dispose();
    }

    bool IndexBufferContainer::Create(ID3D12Device* device, size_t size, size_t stride, const void* srcData)
    {
      if (device == nullptr)
      {
        return false;
      }

      if (size == 0 || stride == 0)
      { 
        return false;
      }

      // 頂点バッファー作成
      {
        HRESULT result = S_OK;
        D3D12_HEAP_PROPERTIES heapProp = {};

        heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;                       // CPUからアクセスできる（Mapできる）※DEFAULTより遅い
        heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;   // TypeはCUSTOMじゃないためUNKNOWNでよい
        heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;    // TypeはCUSTOMじゃないためUNKNOWNでよい
        heapProp.CreationNodeMask = 1; // 0 と同じ意味
        heapProp.VisibleNodeMask = 1;

        // 頂点バッファー情報
        D3D12_RESOURCE_DESC resourceDesc = {};

        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;       // バッファーに使うためBUFFERを指定
        resourceDesc.Alignment = 0;                                     // ??
        resourceDesc.Width = static_cast<UINT64>(size);                 // 頂点情報が入るだけのサイズ
        resourceDesc.Height = 1;                                        // 幅で表現しているので１とする
        resourceDesc.DepthOrArraySize = 1;                              // 1でよい
        resourceDesc.MipLevels = 1;                                     // 1でよい
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;                      // 今回は画像ではないためUNKNOWNでよい
        resourceDesc.SampleDesc.Count = 1;                              // アンチエイリアシングを行うときのパラメーター。※ 0だとデータがないことになってしまうため、1にする
        resourceDesc.SampleDesc.Quality = 0;                            // ??
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;           // UNKNOWNと指定すると、自動で最適なレイアウトに設定しようとする　※今回はテクスチャではないため不適切です
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;                  // NONEでよい※要調べ

        result = device->CreateCommittedResource(
                                                  &heapProp,
                                                  D3D12_HEAP_FLAG_NONE,
                                                  &resourceDesc,
                                                  D3D12_RESOURCE_STATE_GENERIC_READ,
                                                  nullptr,
                                                  IID_PPV_ARGS(m_idxBuffer.ReleaseAndGetAddressOf())
                                                );
        // 作成失敗
        if (FAILED(result))
        {
          return false;
        }

        // ビュー作成
        m_idxBufferView.BufferLocation = m_idxBuffer->GetGPUVirtualAddress();
        m_idxBufferView.SizeInBytes = static_cast<UINT>(size);
        m_idxBufferView.Format = DXGI_FORMAT_R16_UINT;

        // ID3D12Resource::Mapメソッド
        // 第一引数: ミップマップなどではないため0でよい　  ※リソース配列やミップマップの場合、サブリソース番号を渡す
        // 第二引数: 範囲指定。全範囲なのでnullptrでよい   ※一部のみを更新したいときは設定する
        // 第三引数: 受け取るためのポインター変数のアドレス

        if (srcData != nullptr)
        {
          void* idxMap = nullptr;

          result = m_idxBuffer->Map(0, nullptr, &idxMap);

          if (FAILED(result))
          {
            return false;
          }

          memcpy_s(idxMap, size, srcData, size);

          m_idxBuffer->Unmap(0, nullptr);
        }
      }

      return true;
    }

    void IndexBufferContainer::Dispose() noexcept
    {
      m_idxBuffer.Reset();
      memset(&m_idxBufferView, 0, sizeof(m_idxBufferView));
    }
  }
}