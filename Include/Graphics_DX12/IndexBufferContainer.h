/*

MRenderFramework
Author : MAI ZHICONG

Description : IndexBuffer Container (Graphics API: DirectX12)

Update History: 2024/11/08 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_IDXBUFFER_CONTAINER
#define M_IDXTBUFFER_CONTAINER

#include <d3d12.h>

#include <ComPtr.h>
#include <Interfaces/IDisposable.h>
#include <Class-Def-Macro.h>

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class IndexBufferContainer final : public IDisposable
    {
      GENERATE_CLASS_NO_COPY(IndexBufferContainer)

      public:
        bool Create(ID3D12Device*, size_t, size_t, const void*);
        template<typename T>
        bool Create(ID3D12Device*, size_t, const T*);

      public:
        void Dispose(void) noexcept override;
      
      public:
        D3D12_INDEX_BUFFER_VIEW GetView() const;

      private:
        ComPtr<ID3D12Resource> m_idxBuffer;
        D3D12_INDEX_BUFFER_VIEW m_idxBufferView;
    };

    template<typename T>
    inline bool IndexBufferContainer::Create(ID3D12Device* device, size_t size, const T* srcData)
    {
      return Create(device, size, sizeof(T), srcData);
    }

    inline D3D12_INDEX_BUFFER_VIEW IndexBufferContainer::GetView() const
    {
      return m_idxBufferView;
    }
  }
}
#endif