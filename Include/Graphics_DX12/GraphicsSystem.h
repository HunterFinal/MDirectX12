/*

MRenderFramework
Author : MAI ZHICONG

Description : DirectX12 Graphics System (Graphics API: DirectX12)

Update History: 2024/11/12 Create

Version : alpha_1.0.0

Encoding : UTF-8

*/

#pragma once

#ifndef M_GRAPHICS_SYSTEM
#define M_GRAPHICS_SYSTEM

#include <Windows.h>
#include <vector>
#include <Color.h>

#include <Graphics_DX12/GraphicsInclude.h>
#include <Class-Def-Macro.h>

class IGraphics
{
  public:
    virtual void Init(HWND hWnd, bool isDebugMode = false) = 0;
    virtual void PreProcess(void) = 0;
    // TODO
    virtual void Render(void) = 0;
    virtual void PostProcess(void) = 0;
    virtual void Terminate(void) noexcept = 0;

    virtual ~IGraphics() { }
};

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class GraphicsSystem : public IGraphics
    {
      GENERATE_CLASS_NO_COPY(GraphicsSystem)

    // インタフェース実装
    #pragma region Interface implementation
      public:
        void Init(HWND hWnd, bool isDebugMode = false) override;
        void PreProcess(void) override;
        void Render(void) override;
        void PostProcess(void) override;
        void Terminate(void) noexcept override;
    #pragma endregion Interface implementation
    // endregion of Interface implementation
    // Private変数
    #pragma region private variables
      private:
        DX12DXGIFactory m_dxgiFactory;
        DX12Device m_device;
        CommandList m_cmdList;
        CommandQueue m_cmdQueue;
        SwapChain m_swapChain;
        DescriptorHeap m_rtvHeap;
        DescriptorHeap m_texHeap;
        std::vector<RenderTarget> m_renderTargets;
        Fence m_fence;
        ConstantBuffer m_constBuffer;
        VertexBufferContainer m_vertBuffer;
        IndexBufferContainer m_idxBuffer;
        ShaderResBlob m_vertShader;
        ShaderResBlob m_pixelShader;
        RootSignature m_rootSig;
        PipelineState m_pipelineState;

        D3D12_VIEWPORT m_viewPort;
        D3D12_RECT  m_scissorRect;

        Color m_clearColor;

    #pragma endregion private variables
    // endregion of private variables
    };
  }
}

#endif