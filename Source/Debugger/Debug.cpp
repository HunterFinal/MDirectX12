#include <Debug.hpp>

#include <iostream>
#include <cstdarg>

#include <DebugFormat.h>

namespace MDebug
{
    void Debug::Log(const char *format, ...) const
    {
        #ifdef _DEBUG
            va_list valist;
            va_start(valist,format);
            DebugOutputFormatString(format, valist);
            va_end(valist);
        #endif
    }

}
