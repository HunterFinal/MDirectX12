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

// region of DX12 Graphics
#pragma region DX12 Graphics

#include <Graphics_DX12/GraphicsSystem.h>

#pragma endregion
// end region of DX12 Graphics


#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
#endif
{
  int width = GetSystemMetrics(SM_CXSCREEN);
  int height = GetSystemMetrics(SM_CYSCREEN);
  // Window
  MWindow::Window test;
  if(!test.InitWnd(1920, 1080, L"GameWindow", L"Framework_Window_Title"))
  {
    return -1;
  }

  std::shared_ptr<IGraphics> graphics = std::make_shared<MFramework::GraphicsSystem>();
  graphics->Init(test.GetHWND(), true);

  MSG msg = {};

  #pragma region Main Loop
  // メインループ
  while(test.PollWNDMessage(msg))
  {
    graphics->PreProcess();
    graphics->Render();
    graphics->PostProcess();
  } 
  #pragma endregion

  graphics->Terminate();
  graphics.reset();

  test.Dispose();

  return 0;
}
