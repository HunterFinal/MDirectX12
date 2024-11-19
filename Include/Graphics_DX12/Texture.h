/*

MRenderFramework
Author : MAI ZHICONG

Description : Texture of MRenderFramework (Graphics API: DirectX12)

Update History: 2024/11/01

Version : alpha_1.0.0

Encoding : UTF-8 

*/
#pragma once

#ifndef M_TEXTURE
#define M_TEXTURE

#include <ComPtr.h>
#include <Class-Def-Macro.h>
#include <Interfaces/IDisposable.h>
#include <Graphics_DX12/DescriptorHandle.h>

struct ID3D12Resource;
struct ID3D12Device;
struct ID3D12CommandQueue;

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class CommandList;
  }
}

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    
    class Texture final : public IDisposable
    {
      GENERATE_CLASS_NORMAL(Texture)

      public:
        bool Create(ID3D12Device*, 
                    CommandList*,
                    ID3D12CommandQueue*,
                    DescriptorHandle,
                    const wchar_t*);

      public:
        void Dispose(void) noexcept override;

      public:
        ID3D12Resource* Get(void) const;
        CPUDescHandle GetCPUHandle(void) const;
        GPUDescHandle GetGPUHandle(void) const;

      private:
        ComPtr<ID3D12Resource> m_tex;
        DescriptorHandle m_handle;
    };

    inline ID3D12Resource* Texture::Get() const
    {
      return m_tex.Get();
    }

    inline CPUDescHandle Texture::GetCPUHandle() const
    {
      if (m_handle.HasCPUHandle())
      {
        return m_handle.CPUHandle;
      }
      else
      {
        return CPUDescHandle();
      }
    }

    inline GPUDescHandle Texture::GetGPUHandle() const
    {
      if (m_handle.HasGPUHandle())
      {
        return m_handle.GPUHandle;
      }
      else
      {
        return GPUDescHandle();
      }
    }
  }
}

#endif