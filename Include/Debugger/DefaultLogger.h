/*

MRenderFramework.DebugFormat
Author : MAI ZHICONG

Description : Definition of debug log format

Update History: 2024/09/19 Create

Version : alpha_1.0.0

*/

#pragma once

#ifndef M_DEFAULT_LOGGER
#define M_DEFAULT_LOGGER

#include <ILogger.h>

class DefaultLogger : public ILogger
{

public:
    void LogFormat(const char* format, va_list args) override;
    void LogWarningFormat(const char* format, va_list args) override;
    void LogErrorFormat(const char* format, va_list args) override;

};

#endif // _DEFAULT_LOGGER
