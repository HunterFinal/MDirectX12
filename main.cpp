/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderFramework used by Game (Graphics API: DirectX12)

Update History: 2024/09/19 Create

Version : alpha_1.0.0

*/

// DirectX 12、シェーダー モデル 5.1、D3DCompile API、FXC はすべて非推奨です。 
// 代わりに、DXIL 経由でシェーダー モデル 6 を使用してください。 GitHub を参照してください。
// url https://github.com/microsoft/DirectXShaderCompiler

// Debug include
#ifdef _DEBUG

#include <DebugHelper>

#endif
// end Debug include

// MWindow include
#include <Window/BaseWindow.h>
#include <Interfaces/IWindowInfo.h>
// end MWindow include

// DX12 include
#include <d3d12.h>
#include <dxgi1_6.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <DirectXMath.h>

// end DX12 include

// shader compiler include
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

// end shader compiler include

// DirectXTex lib include

#include <DirectXTex.h>

#pragma comment(lib, "DirectXTex.lib")

// end DirectXTex lib include

#include <vector>
#include <string>

#pragma region DX12 Wrapper

#include <Graphics_DX12/DX12Device.h>
#include <Graphics_DX12/DX12DXGIFactory.h>
#include <Graphics_DX12/CommandList.h>
#include <Graphics_DX12/CommandQueue.h>
#include <Graphics_DX12/RootSignature.h>
#include <Graphics_DX12/PipelineState.h>
#include <Graphics_DX12/DX12SwapChain.h>
#include <Graphics_DX12/Fence.h>

#include <Graphics_DX12/VertexBufferContainer.h>
#include <Graphics_DX12/IndexBufferContainer.h>

#include <Graphics_DX12/DescriptorHandle.h>
#include <Graphics_DX12/DescriptorHeap.h>
#include <Graphics_DX12/ConstantBuffer.h>

#include <Graphics_DX12/RenderTarget.h>

#include <Graphics_DX12/ShaderResBlob.h>

#include <Graphics_DX12/GraphicsSystem.h>

#pragma endregion
// end region of DX12 Wrapper

#include <Graphics_DX12/Texture.h>
#include <RandomGenerator.hpp>

struct Vertex
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 uv;
};

/// @brief
/// アライメントに揃えたサイズを返す
/// @param size 元のサイズ
/// @param alignment アラインメントサイズ
/// @return アラインメントを揃えたサイズ
size_t AlignmentedSize(size_t size, size_t alignment)
{
  return size + alignment - size % alignment;
}

#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
#endif
{
    // Window
    MWindow::Window test;

    if(!test.InitWnd(1920, 1080, L"GameWindow", L"Framework_Window_Title"))
    {
        return -1;
    }

    UINT frame = 0;

    MSG msg = {};


    IGraphics* g = new MFramework::GraphicsSystem();
    g->Init(test.GetHWND(), true);

    #pragma region Main Loop
    // メインループ
    while(test.PollWNDMessage(msg))
    {
        g->PreProcess();
        g->Render();
        g->PostProcess();
    } 
   
    g->Terminate();
    delete g;

    return 0;
}
