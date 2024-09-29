/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderFramework used by Game (Graphics API: DirectX12)

Update History: 2024/09/19 Create
                2024/09/26 Update constructor
                           Create virtual WndProc

Version : alpha_1.0.0

Encoding : UTF-8 

*/
#pragma once

#ifndef _M_COM_PTR
#define _M_COM_PTR

#include <wrl.h>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

#endif