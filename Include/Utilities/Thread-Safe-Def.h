/*

MFramework

Author : MAI ZHICONG

Description : Thread save define

Update History: 2024/11/21 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_THREAD_SAVE
#define M_THREAD_SAVE

#include <mutex>

#define LOCK(padlock) { std::lock_guard<std::mutex>guard(padlock);}

#endif