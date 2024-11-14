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

#include <memory>
// Debug include
#ifdef _DEBUG

#include <DebugHelper>

#endif
// end Debug include

// MWindow include
#include <Window/BaseWindow.h>
#include <Interfaces/IWindowInfo.h>
// end MWindow include

// DX12 Graphics
#pragma region DX12 Graphics
#include <Graphics_DX12/GraphicsSystem.h>
#pragma endregion
// end DX12 Graphics

#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
#endif
{
    // Window
    MWindow::Window test;

    if(!test.InitWnd(1920, 1080, L"GameWindow", L"Framework_Window_Title"))
    {
        return -1;
    }
    MSG msg = {};

    std::shared_ptr<IGraphics> g = std::make_shared<MFramework::GraphicsSystem>();
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
    g.reset();

    return 0;
}
