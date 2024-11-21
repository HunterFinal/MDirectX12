/*

MRenderFramework
Author : MAI ZHICONG

Description : VertexBuffer Container (Graphics API: DirectX12)

Update History: 2024/09/19 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/
#pragma once

#ifndef M_VERTBUFFER_CONTAINER
#define M_VERTBUFFER_CONTAINER

#include <d3d12.h>

#include "GraphicsClassBaseInclude.h"

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class VertexBufferContainer final : public IDisposable
    {
      GENERATE_CLASS_NO_COPY(VertexBufferContainer)

      public:
        bool Create(ID3D12Device*, size_t size, size_t stride, const void* srcData);
        template<typename T>
        bool Create(ID3D12Device*, size_t size, const T* srcData);

      public:
        void Dispose(void) noexcept override;
      
      public:
        D3D12_VERTEX_BUFFER_VIEW GetView() const;

      private:
        ComPtr<ID3D12Resource> m_vertBuffer;
        D3D12_VERTEX_BUFFER_VIEW m_vertBufferView;
    };

    template<typename T>
    inline bool VertexBufferContainer::Create(ID3D12Device* device, size_t size, const T* srcData)
    {
      return Create(device, size, sizeof(T), srcData);
    }

    inline D3D12_VERTEX_BUFFER_VIEW VertexBufferContainer::GetView() const
    {
      return m_vertBufferView;
    }
  }
}
#endif