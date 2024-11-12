/*

MRenderFramework
Author : MAI ZHICONG

Description : DirectX12 Graphics System (Graphics API: DirectX12)

Update History: 2024/11/12 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_GRAPHICS_SYSTEM
#define M_GRAPHICS_SYSTEM

#include <Graphics_DX12/GraphicsInclude.h>

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class GraphicsSystem
    {

      private:
        DX12DXGIFactory m_dxgiFactory;
        DX12Device m_device;
        CommandList m_cmdList;
        CommandQueue m_cmdQueue;
    };
  }
}

#endif