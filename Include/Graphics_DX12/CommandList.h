/*

MRenderFramework
Author : MAI ZHICONG

Description : DirectX12 CommandList Wrapper (Graphics API: DirectX12)

Update History: 2024/11/06 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_DX12_COMMANDLIST
#define M_DX12_COMMANDLIST

#include <ComPtr.h>
#include <vector>
#include <Class-Def-Macro.h>
#include <Interfaces/IDisposable.h>

struct ID3D12Device;
struct ID3D12GraphicsCommandList;
struct ID3D12CommandAllocator;
struct ID3D12PipelineState;

enum D3D12_COMMAND_LIST_TYPE;

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class CommandList final : public IDisposable
    {
      GENERATE_CLASS_NO_COPY(CommandList)

      public:
        void Init(ID3D12Device*, D3D12_COMMAND_LIST_TYPE, int);
        void Reset(int, ID3D12PipelineState* = nullptr);

      public:
        void Dispose(void) noexcept override;

      public:
        ID3D12GraphicsCommandList* Get(void) const;
        ID3D12GraphicsCommandList* operator->() const noexcept;

      private:
        ComPtr<ID3D12GraphicsCommandList> m_commandList;
        std::vector<ComPtr<ID3D12CommandAllocator>> m_commandAllocators;

    };

    inline ID3D12GraphicsCommandList* CommandList::Get(void) const
    {
      return m_commandList.Get();
    }

    inline ID3D12GraphicsCommandList* CommandList::operator->() const noexcept
    {
      return m_commandList.Get();
    }

  }
}

#endif
