/*

MRenderFramework
Author : MAI ZHICONG

Description : ComPtr Alias

Update History: 2024/09/19 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/
#pragma once

#ifndef M_COM_PTR
#define M_COM_PTR

#include <wrl/client.h>

template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

#endif