/*

MRenderFramework
Author : MAI ZHICONG

Description : D3D12Library Easy Utilities

Update History: 2024/11/19 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_D3D12_EASY_UTIL
#define M_D3D12_EASY_UTIL

namespace MFramework
{
  inline namespace Utility
  {
    class D3D12EasyUtility final
    {
      public:
        /// @brief
        /// アライメントに揃えたサイズを返す
        /// @param size 元のサイズ
        /// @param alignment アラインメントサイズ
        /// @return アラインメントを揃えたサイズ
        static size_t AlignmentedSize(size_t size, size_t alignment);


      private:
        D3D12EasyUtility() = delete;
    };
  }
}

#endif