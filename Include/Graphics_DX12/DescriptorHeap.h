/*

MRenderFramework
Author : MAI ZHICONG

Description : ID3D12DescriptorHeap Wrapper (Graphics API: DirectX12)

Update History: 2024/11/12 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/
#pragma once

#ifndef M_DX12_DESCHEAP
#define M_DX12_DESCHEAP

#include "GraphicsClassBaseInclude.h"

#include <Graphics_DX12/DescriptorHandle.h>

enum class D3D12DescHeapType
{
  None = -1,
  CBV_SRV_UAV = 0,
  SAMPLER = (CBV_SRV_UAV + 1),
  RTV = (SAMPLER + 1),
  DSV = (RTV + 1),
  NUM_TYPES = (DSV + 1),
};

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class DescriptorHeap final : public IDisposable
    {
      GENERATE_CLASS_NO_COPY(DescriptorHeap)

      public:
        void Init(ID3D12Device*, D3D12DescHeapType, size_t numDesc);
        ID3D12DescriptorHeap* Get(void) const;
        DescriptorHandle GetHandle(size_t index) const;
        D3D12DescHeapType GetHeapType(void) const;
        size_t GetHeapNum(void) const;

      public:
        void Dispose(void) noexcept override;

      private:
        ComPtr<ID3D12DescriptorHeap> m_descHeap;
        D3D12DescHeapType m_heapType;
        size_t m_numDesc;
        UINT m_incrementSize;
    };

    inline ID3D12DescriptorHeap* DescriptorHeap::Get() const
    { 
      return m_descHeap.Get();
    }

    inline D3D12DescHeapType DescriptorHeap::GetHeapType() const
    {
      return m_heapType;
    }

    inline size_t DescriptorHeap::GetHeapNum() const
    {
      return m_numDesc;
    }

  }
}
#endif