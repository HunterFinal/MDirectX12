/*

MRenderFramework
Author : MAI ZHICONG

Description : PipelineState Wrapper (Graphics API: DirectX12)

Update History: 2024/11/12 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_DX12_PIPELINESTATE
#define M_DX12_PIPELINESTATE

#include "GraphicsClassBaseInclude.h"

struct ID3D12Device;
struct ID3D12RootSignature;
struct ID3D12PipelineState;

struct D3D12_BLEND_DESC;

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class ShaderResBlob;
  }
}

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class PipelineState final : public IDisposable
    {
      GENERATE_CLASS_NO_COPY(PipelineState)

      public:
        void Init
                  (
                    ID3D12Device*,
                    ID3D12RootSignature*,
                    ShaderResBlob* vertexShader,
                    ShaderResBlob* pixelShader,
                    D3D12_BLEND_DESC*
                  );
        ID3D12PipelineState* Get(void) const;

      public:
        void Dispose(void) noexcept override;

      private:
        ComPtr<ID3D12PipelineState> m_pipelineState;
    };

    inline ID3D12PipelineState* PipelineState::Get() const
    {
      return m_pipelineState.Get();
    }
  }
}

#endif