/*

MRenderFramework
Author : MAI ZHICONG

Description : File Utilities

Update History: 2024/11/10 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_FILE_UTIL
#define M_FILE_UTIL

#include <string>

namespace MFramework
{
  inline namespace Utility
  {
    class FileUtility final
    {
      private:
        FileUtility() = delete;
      public:
        static bool SearchFilePath(const wchar_t* fileName, std::wstring& filePath);
    };
  }
}

#endif