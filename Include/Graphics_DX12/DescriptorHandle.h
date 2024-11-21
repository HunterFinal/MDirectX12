/*

MRenderFramework
Author : MAI ZHICONG

Description : CPU and GPU Descriptor Handle Wrapper (Graphics API: DirectX12)

Update History: 2024/11/10 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_DX12_DESC_HANDLE
#define M_DX12_DESC_HANDLE

#include <d3d12.h>
#include <Base-Macro-Def.h>

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    ALIAS(D3D12_CPU_DESCRIPTOR_HANDLE, CPUDescHandle);
    ALIAS(D3D12_GPU_DESCRIPTOR_HANDLE, GPUDescHandle);
    
    class DescriptorHandle
    {
      public:
        DescriptorHandle();
        ~DescriptorHandle();
      public:
        CPUDescHandle CPUHandle;
        GPUDescHandle GPUHandle;

      public:
        bool HasCPUHandle(void) const;
        bool HasGPUHandle(void) const;
    };

    inline bool DescriptorHandle::HasCPUHandle() const
    {
      return CPUHandle.ptr != 0;
    }

    inline bool DescriptorHandle::HasGPUHandle() const
    {
      return GPUHandle.ptr != 0;
    }
  }
}

#endif