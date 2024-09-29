#include <Graphics_DX12/Fence.h>

namespace MFramework
{
    namespace MGraphics_DX12
    {
        Fence::Fence()
            : m_fence(nullptr)
            , m_event(nullptr)
            , m_fenceCount(0)
            , m_isInitialized(false)
        {

        }

        Fence::~Fence()
        {
            Terminate();
        }

        bool Fence::Init(ID3D12Device* device)
        {
            if (device == nullptr)
            {
                return false;
            }

            // TODO CreateEventEx
            m_event = CreateEvent(nullptr, false, false, nullptr);

            if (m_event == nullptr)
            {
                return false;
            }

            auto hResult = device->CreateFence(  
                                                m_fenceCount, 
                                                D3D12_FENCE_FLAG_NONE, 
                                                IID_PPV_ARGS(m_fence.GetAddressOf())
                                              );

            if (FAILED(hResult))
            {
                Terminate();
                return false;
            }

            m_isInitialized = true;

            return true;
        }

        void Fence::Wait(ID3D12CommandQueue* commandQueue, UINT32 waitTime)
        {
            if (commandQueue == nullptr)
            {
                return;
            }

            if (!m_isInitialized)
            {
                return;
            }

            commandQueue->Signal(m_fence.Get(), ++m_fenceCount);

            if(m_fence->GetCompletedValue() != m_fenceCount)
            {
                m_fence->SetEventOnCompletion(m_fenceCount, m_event);

                // イベントが発生するまで待ち続ける
                WaitForSingleObject(m_event, waitTime);

            }

        }

        void Fence::Terminate() noexcept
        {
            if (m_event != nullptr)
            {
                CloseHandle(m_event);
                m_event = nullptr;
            }

            m_fence.Reset();

            m_fenceCount = 0;

            m_isInitialized = false;
        }
    }
}