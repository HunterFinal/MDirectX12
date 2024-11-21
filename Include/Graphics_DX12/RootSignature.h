/*

MRenderFramework
Author : MAI ZHICONG

Description : DirectX12 RootSignature Wrapper (Graphics API: DirectX12)

Update History: 2024/11/07 Create
           
Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_DX12_ROOTSIGNATURE
#define M_DX12_ROOTSIGNATURE

#include "GraphicsClassBaseInclude.h"

struct ID3D12Device;
struct ID3D12RootSignature;

enum D3D12_FILTER;

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class RootSignature final : public IDisposable
    {
      GENERATE_CLASS_NO_COPY(RootSignature)

      public:
        void Init(ID3D12Device*, D3D12_FILTER);

      public:
        void Dispose(void) noexcept override;

      public:
        ID3D12RootSignature* Get(void) const;
        ID3D12RootSignature* operator->() const noexcept;

      private:
        ComPtr<ID3D12RootSignature> m_rootSignature;

    };

    inline ID3D12RootSignature* RootSignature::Get(void) const
    {
      return m_rootSignature.Get();
    }

    inline ID3D12RootSignature* RootSignature::operator->() const noexcept
    {
      return m_rootSignature.Get();
    }
  }
}

#endif