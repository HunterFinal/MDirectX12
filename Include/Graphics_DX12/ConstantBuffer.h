/*

MRenderFramework
Author : MAI ZHICONG

Description : Constant Buffer (Graphics API: DirectX12)

Update History: 2024/11/10 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_DX12_CONSTANT_BUFFER
#define M_DX12_CONSTANT_BUFFER

#include "GraphicsClassBaseInclude.h"

#include <d3d12.h>
#include <Graphics_DX12/DescriptorHandle.h>

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class ConstantBuffer final : public IDisposable
    {
      GENERATE_CLASS_NO_COPY(ConstantBuffer)

      public:
        bool Create(
                      ID3D12Device*,
                      const DescriptorHandle&,
                      size_t size,
                      const void* srcData = nullptr
                   );      
        template<typename T>
        bool Create(
                      ID3D12Device*,
                      const DescriptorHandle&,
                      int dataCount,
                      const T* srcData
                   );
        
        void Remap(size_t size, const void* srcData);
        template<typename T>
        void Remap(size_t dataCount, const T* srcData);

      public:
        void Dispose(void) noexcept override;

      private:
        void map(size_t, const void*);
        void unmap(void);
      private:
        ComPtr<ID3D12Resource> m_constantBuffer;
        D3D12_CONSTANT_BUFFER_VIEW_DESC m_viewDesc;
        DescriptorHandle m_descHandle;
        void* m_mappedData;
        bool m_isMapped;
    };

    template<typename T>
    inline bool ConstantBuffer::Create(ID3D12Device* device, const DescriptorHandle& handle, int dataCount, const T* srcData)
    {
      return Create(device, handle, sizeof(T) * static_cast<size_t>(dataCount), reinterpret_cast<const void*>(srcData));
    }

    template<typename T>
    inline void ConstantBuffer::Remap(size_t dataCount, const T* srcData)
    {
      Remap(sizeof(T) * dataCount, reinterpret_cast<const void*>(srcData));
    }
  }
}

#endif