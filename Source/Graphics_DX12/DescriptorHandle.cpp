/*

MRenderFramework
Author : MAI ZHICONG

Description : CPU and GPU Descriptor Handle Wrapper (Graphics API: DirectX12)

Update History: 2024/11/10 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <Graphics_DX12/DescriptorHandle.h>

namespace MFramework
{
  DescriptorHandle::DescriptorHandle()
  {
    memset(&CPUHandle, 0, sizeof(CPUHandle));
    memset(&GPUHandle, 0, sizeof(GPUHandle));
  }
  DescriptorHandle::~DescriptorHandle()
  {
    memset(&CPUHandle, 0, sizeof(CPUHandle));
    memset(&GPUHandle, 0, sizeof(GPUHandle));
  }
}