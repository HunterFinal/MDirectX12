/*

MRenderFramework.DebugFormat
Author : MAI ZHICONG

Description : Definition of debug log format

Update History: 2024/09/19 Create

Version : alpha_1.0.0

*/

#include <DefaultLogger.h>

#include <iostream>
#include <cstdarg>

#include <Windows.h>

void DefaultLogger::LogFormat(const char* format, va_list args)
{
    #ifdef _DEBUG
        vprintf_s(format, args);
    #endif
}

void DefaultLogger::LogWarningFormat(const char* format, va_list args)
{
}

void DefaultLogger::LogErrorFormat(const char* format, va_list args)
{
}
