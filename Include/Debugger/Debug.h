/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderFramework used by Game (Graphics API: DirectX12)

Update History: 2024/09/19 Create

Version : alpha_1.0.0

*/

#pragma once
#ifndef M_DEBUGGER
#define M_DEBUGGER 1

#include <ILogger.h>

namespace MDebug
{
    class Debug
    {
        public:
            void Log(const char* format,...) const;
            void LogWarning(const char* format, ...) const;
            void LogError(const char* format, ...) const;

            void SetLogger(ILogger* logger);

        public: 
            Debug(ILogger* logger = nullptr);
            ~Debug();

        private:
            Debug(const Debug& other) = delete;
            Debug& operator=(const Debug& other) = delete;

        // デバッグログ出力実装部分
        private:
            ILogger* m_logger;
    };
}
#endif // _DEBUGGER