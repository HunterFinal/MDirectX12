/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderFramework used by Game (Graphics API: DirectX12)

Update History: 2024/09/19 Create
                2024/09/26 Update constructor
                           Create virtual WndProc

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <Graphics_DX12/CommandList.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#include <cassert>

// TODO
// D3D12_COMMAND_LIST_TYPE_DIRECT
// 値: 0
// GPU が実行できるコマンドバッファーを指定します。直接コマンドリストは、どのようなGPU状態も継承しません。
// D3D12_COMMAND_LIST_TYPE_BUNDLE
// 値: 1
// 直接コマンドリストを介してのみ直接実行できるコマンドバッファーを指定します。バンドルコマンドリストは、
// すべてのGPU状態を継承します(現在設定されているパイプライン状態オブジェクトとプリミティブトポロジを除く)。
// D3D12_COMMAND_LIST_TYPE_COMPUTE
// 値: 2
// コンピューティング用のコマンドバッファーを指定します。
// D3D12_COMMAND_LIST_TYPE_COPY
// 値: 3
// コピーするコマンドバッファーを指定します。
// D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE
// 値: 4
// ビデオ デコード用のコマンドバッファーを指定します。
// D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS
// 値: 5
// ビデオ処理用のコマンドバッファーを指定します。
namespace MFramework
{
  CommandList::CommandList()
    : m_commandList(nullptr)
    , m_commandAllocators()
  { }

  CommandList::~CommandList()
  {
    Dispose();
  }
  void CommandList::Init(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type, int frameCount)
  {
    if (m_commandList.Get() != nullptr)
    {
      return;
    }

    if (device == nullptr)
    {
      return;
    }
    
    if (frameCount <= 0)
    {
      return;
    }

    m_commandAllocators.resize(frameCount);

    // コマンドリストを作成
    HRESULT result = S_OK;

    for (int i = 0; i < frameCount; ++i)
    {
      result = device->CreateCommandAllocator(type, IID_PPV_ARGS(m_commandAllocators[i].GetAddressOf()));
      assert(SUCCEEDED(result));
    }

    result = device->CreateCommandList(
                                        0,                                          // NodeMask(単一GPU操作する場合0でよい)
                                        type,                                       // CommandList種類
                                        m_commandAllocators[0].Get(),               // CommandAllocatorのポインタ
                                        nullptr,                                    // コマンドリストの初期パイプライン状態を含むパイプライン状態オブジェクトへの省略可能なポインター
                                        IID_PPV_ARGS(m_commandList.GetAddressOf()) 
                                      );

    assert(SUCCEEDED(result));
  }

  void CommandList::Reset(int frameIndex, ID3D12PipelineState* pipelineState)
  {
    if (frameIndex < 0 || frameIndex >= static_cast<int>(m_commandAllocators.size()))
    {
      return;
    }

    HRESULT result = S_OK;

    result = m_commandAllocators[frameIndex]->Reset();

    if (FAILED(result))
    {
      return;
    }

    m_commandList->Reset(m_commandAllocators[frameIndex].Get(), pipelineState);
  }

  void CommandList::Dispose() noexcept
  {
    m_commandList.Reset();

    for(size_t i = 0; i < m_commandAllocators.size(); ++i)
    {
      m_commandAllocators[i].Reset();
    }

    m_commandAllocators.clear();
    m_commandAllocators.shrink_to_fit();
  }

    
}
