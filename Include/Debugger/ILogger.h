/*

MRenderFramework.DebugFormat
Author : MAI ZHICONG

Description : Definition of debug log format

Update History: 2024/09/19 Create

Version : alpha_1.0.0

*/

#pragma once

#ifndef M_LOGGER
#define M_LOGGER 1

#include <cstdarg>

// Implementation(Bridge Pattern)
class ILogger
{
public:
    virtual void LogFormat(const char* format, va_list args) = 0;
    virtual void LogWarningFormat(const char* format, va_list args) = 0;
    virtual void LogErrorFormat(const char* format, va_list args) = 0;
};

#endif // _LOGGER