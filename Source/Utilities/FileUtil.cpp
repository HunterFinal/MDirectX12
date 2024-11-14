/*

MRenderFramework
Author : MAI ZHICONG

Description : File Utilities

Update History: 2024/11/10 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <FileUtil.h>

#include <wchar.h>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

namespace
{
  constexpr int DESTINATION_PATH_BUFFER_LENGTH = 520;
}

namespace MFramework
{
  bool FileUtility::SearchFilePath(const wchar_t* fileName, std::wstring& filePath)
  {
    if (fileName == nullptr)
    {
      return false;
    }

    if (wcscmp(fileName, L"") == 0 || (fileName[0] == (L" "[0])))
    {
      return false;
    }

    wchar_t exePath[DESTINATION_PATH_BUFFER_LENGTH] = {};
    GetModuleFileName(nullptr, exePath, DESTINATION_PATH_BUFFER_LENGTH);
    exePath[DESTINATION_PATH_BUFFER_LENGTH - 1] = L'\0';
    PathRemoveFileSpec(exePath);

    wchar_t destPath[DESTINATION_PATH_BUFFER_LENGTH] = {};

    wcscpy_s(destPath, fileName);
    if (PathFileExists(destPath))
    {
      filePath = destPath;
      return true;
    }
    
    swprintf_s(destPath, L"%s\\%s", exePath, fileName);
    if (PathFileExists(destPath) == TRUE) {
      filePath = destPath;
      return true;
    }

    return false;
  }
}