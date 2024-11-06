/*

MRenderFramework
Author : MAI ZHICONG

Description : DirectX12 CommandQueue Wrapper (Graphics API: DirectX12)

Update History: 2024/11/06 Create
           
Version : alpha_1.0.0

Encoding : UTF-8 

*/
#include <Graphics_DX12/CommandQueue.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#include <cassert>

namespace MFramework
{
  CommandQueue::CommandQueue()
    : m_commandQueue(nullptr)
  { }
  
  CommandQueue::~CommandQueue()
  {
    Dispose();
  }

  void CommandQueue::Init(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
  {
    if (m_commandQueue.Get() != nullptr)
    {
      return;
    }

    if (device == nullptr)
    {
      return;
    }

    D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
    // タイムアウトなし
    cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    // アタブターを一つしか使わないときは0でよい
    cmdQueueDesc.NodeMask = 0;
    // 優先順位は特に指定なし
    cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    // コマンドリストと合わせる
    cmdQueueDesc.Type = type;

    HRESULT result = S_OK;
    result = device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(m_commandQueue.GetAddressOf()));

    assert(SUCCEEDED(result));
  }

  void CommandQueue::Execute(int numCommandList, ID3D12CommandList* const *commandLists)
  {
    if (m_commandQueue.Get() == nullptr)
    {
      return;
    }

    if (numCommandList <= 0)
    {
      return;
    }

    if (commandLists == nullptr)
    {
      return;
    }

    m_commandQueue->ExecuteCommandLists(numCommandList, commandLists);
  }

  void CommandQueue::Dispose() noexcept
  {
    m_commandQueue.Reset();
  }
}