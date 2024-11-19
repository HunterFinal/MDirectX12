/*

MRenderFramework
Author : MAI ZHICONG

Description : D3D12Library Easy Utilities

Update History: 2024/11/19 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <D3D12EasyUtil.h>

namespace MFramework
{
  size_t D3D12EasyUtility::AlignmentedSize(size_t size, size_t alignment)
  {
    return size + alignment - size % alignment;
  }
}