/*

MGameEngine Core Module
Author : MAI ZHICONG

Description : Color (0.0f~1.0f)

Update History: 2024/11/13 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_COLORF
#define M_COLORF

namespace MGameEngine
{
  inline namespace CoreModule
  {
    struct Color final
    {
      float r;
      float g;
      float b;
      float a;

      static const Color black;
    };
  }
}

using Color = MGameEngine::CoreModule::Color;
using Colorf = Color;
using ColorF = Color;

#endif