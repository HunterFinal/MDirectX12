#include <DebugFormat.h>

#include <iostream>
#include <cstdarg>

namespace MDebug
{
    void DebugOutputFormatString(const char *format, ...)
    {   
        #ifdef _DEBUG
            va_list vaList;
            va_start(vaList,format);
            printf_s(format,vaList);
            va_end(vaList);
        #endif
    }
}