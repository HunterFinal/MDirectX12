/*

MRenderFramework
Author : MAI ZHICONG

Description : DirectX12 CommandQueue Wrapper (Graphics API: DirectX12)

Update History: 2024/11/06 Create
           
Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_DX12_COMMAND_QUEUE
#define M_DX12_COMMAND_QUEUE

#include <ComPtr.h>
#include <Class-Def-Macro.h>
#include <Interfaces/IDisposable.h>

struct ID3D12Device;
struct ID3D12CommandQueue;
struct ID3D12CommandList;

enum D3D12_COMMAND_LIST_TYPE;

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class CommandQueue final : public IDisposable
    {
      GENERATE_CLASS_NO_COPY(CommandQueue)

      public:
        /// @brief 
        /// 初期化する
        /// @param  
        void Init(ID3D12Device*, D3D12_COMMAND_LIST_TYPE);
        void Execute(int, ID3D12CommandList* const*);

      public:
        void Dispose(void) noexcept override;

      public:
        ID3D12CommandQueue* GetCommandQueue(void) const;
        operator ID3D12CommandQueue*() const noexcept;
        ID3D12CommandQueue* operator->() const noexcept;
      private:
        ComPtr<ID3D12CommandQueue> m_commandQueue;
    };
  }
  // MGraphics_DX12

  inline ID3D12CommandQueue* CommandQueue::GetCommandQueue() const
  {
    return m_commandQueue.Get();
  }

  inline CommandQueue::operator ID3D12CommandQueue*() const noexcept
  {
    return m_commandQueue.Get();
  }

  inline ID3D12CommandQueue* CommandQueue::operator->() const noexcept
  {
    return m_commandQueue.Get();
  }
}
// MFramework

#endif
// M_DX12_DEVICE
