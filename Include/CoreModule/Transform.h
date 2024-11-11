/*

MGameEngine Core Module
Author : MAI ZHICONG

Description : Transform

Update History: 2024/11/11 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_TRANSFORM
#define M_TRANSFORM

#include <Vector3.h>

namespace MGameEngine
{
  inline namespace CoreModule
  {
    class Transform
    {
      private:
        Vector3 m_worldPosition;
        Vector3 m_worldRotation;
        Vector3 m_localScale;
    };
  }
}

#endif