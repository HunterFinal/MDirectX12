#include <Debug.h>

#include <iostream>
#include <cstdarg>

#include <DefaultLogger.h>

namespace MDebug
{
    void Debug::Log(const char *format, ...) const
    {
        #ifdef _DEBUG
        if (m_logger == nullptr)
        {
            return;
        }

        va_list vaList;
        va_start(vaList,format);
        m_logger->LogFormat(format, vaList);
        va_end(vaList);

        #endif
    }

    Debug::Debug(ILogger* logger)
    {
        if (logger != nullptr)
        {
            m_logger = logger;
        }
        else
        {
            m_logger = new DefaultLogger();
        }
    }

    Debug::~Debug()
    {
        if (m_logger != nullptr)
        {
            delete m_logger;
            m_logger = nullptr;
        }
    }

}
