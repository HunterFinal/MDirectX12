/*

MRenderFramework
Author : MAI ZHICONG

Description : IWindowInfo interface

Update History: 2024/11/07 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_WINDOW_INFO
#define M_WINDOW_INFO

#include <Windows.h>

namespace MWindow
{
  class IWindowInfo
  {
    public:
      virtual HWND GetHWND(void) const = 0;
      virtual UINT32 GetWidth(void) const = 0;
      virtual UINT32 GetHeight(void) const = 0;

      virtual ~IWindowInfo() {}
  };
}

#endif