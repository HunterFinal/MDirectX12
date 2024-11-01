/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderFramework used by Game (Graphics API: DirectX12)

Update History: 2024/09/19 Create
                2024/09/26 Update constructor
                           Create virtual WndProc

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <Graphics_DX12/CommandList.h>
#include <cassert>

namespace
{
    constexpr D3D12_COMMAND_LIST_TYPE M_COMMAND_LIST_TYPE = D3D12_COMMAND_LIST_TYPE_DIRECT;
    constexpr float CLEAR_COLOR_R = 1.0f;
    constexpr float CLEAR_COLOR_G = 1.0f;
    constexpr float CLEAR_COLOR_B = 0.0f;
    constexpr float CLEAR_COLOR_A = 1.0f;
    constexpr float CLEAR_COLOR[] = {
                                        CLEAR_COLOR_R,
                                        CLEAR_COLOR_G,
                                        CLEAR_COLOR_B,
                                        CLEAR_COLOR_A
                                    };
}
namespace MFramework
{
    namespace MGraphics_DX12
    {
        CommandList::CommandList()
            : m_commandList(nullptr)
            , m_commandAllocator(nullptr)
            , m_viewport({})
            , m_scissorRect({})
        {

        }
        bool CommandList::Init(ID3D12Device* device)
        {
            assert(device != nullptr);
            // TODO
            // D3D12_COMMAND_LIST_TYPE_DIRECT
            // 値: 0
            // GPU が実行できるコマンドバッファーを指定します。直接コマンドリストは、どのようなGPU状態も継承しません。
            // D3D12_COMMAND_LIST_TYPE_BUNDLE
            // 値: 1
            // 直接コマンドリストを介してのみ直接実行できるコマンドバッファーを指定します。バンドルコマンドリストは、
            // すべてのGPU状態を継承します(現在設定されているパイプライン状態オブジェクトとプリミティブトポロジを除く)。
            // D3D12_COMMAND_LIST_TYPE_COMPUTE
            // 値: 2
            // コンピューティング用のコマンドバッファーを指定します。
            // D3D12_COMMAND_LIST_TYPE_COPY
            // 値: 3
            // コピーするコマンドバッファーを指定します。
            // D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE
            // 値: 4
            // ビデオ デコード用のコマンドバッファーを指定します。
            // D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS
            // 値: 5
            // ビデオ処理用のコマンドバッファーを指定します。
            //
            // コマンドリストを作成
            HRESULT result = device->CreateCommandAllocator(M_COMMAND_LIST_TYPE, IID_PPV_ARGS(m_commandAllocator.GetAddressOf()));

            if (FAILED(result))
            {
                return false;
            }

            result = device->CreateCommandList(
                                                0,                                          // NodeMask(単一GPU操作する場合0でよい)
                                                M_COMMAND_LIST_TYPE,                        // CommandList種類
                                                m_commandAllocator.Get(),                   // CommandAllocatorのポインタ
                                                nullptr,                                    // コマンドリストの初期パイプライン状態を含むパイプライン状態オブジェクトへの省略可能なポインター
                                                IID_PPV_ARGS(m_commandList.GetAddressOf()) 
                                              );

            if (FAILED(result))
            {
                return false;
            }

            return true;
        }

        void CommandList::SetVertexBuffer(UINT startIndex, UINT vertBufferCnt, D3D12_VERTEX_BUFFER_VIEW* bufferView)
        {
            // 初期化していないため、関数を終了させる
            if (m_commandList == nullptr)
            {
                return;
            }

            if (bufferView == nullptr)
            {
                return;
            }

            m_commandList->IASetVertexBuffers(startIndex, vertBufferCnt, bufferView);
        }
        void CommandList::SetIndexBuffers(D3D12_INDEX_BUFFER_VIEW* indexBufferView)
        {
            // 初期化していないため、関数を終了させる
            if (m_commandList == nullptr)
            {
                return;
            }

            if (indexBufferView == nullptr)
            {
                return;
            }

            m_commandList->IASetIndexBuffer(indexBufferView);
        }

        void CommandList::SetBarrier(UINT barrierCnt, D3D12_RESOURCE_BARRIER* barrier)
        {
            // 初期化していないため、関数を終了させる
            if (m_commandList == nullptr)
            {
                return;
            }

            if (barrier == nullptr)
            {
                return;
            }

            m_commandList->ResourceBarrier(barrierCnt, barrier);
        }

        void CommandList::SetPipelineState(ID3D12PipelineState* pipelineState)
        {
            // 初期化していないため、関数を終了させる
            if (m_commandList == nullptr)
            {
                return;
            }

            if (pipelineState == nullptr)
            {
                return;
            }

            m_commandList->SetPipelineState(pipelineState);
        }

        void CommandList::SetRenderTargets(
                                            UINT targetCnt,
                                            D3D12_CPU_DESCRIPTOR_HANDLE* heapStart,
                                            bool RTsSingleHandleToDescriptorRange, 
                                            D3D12_CPU_DESCRIPTOR_HANDLE* depthStencilDesctriptor
                                          )
        {
            // 初期化していないため、関数を終了させる
            if (m_commandList == nullptr)
            {
                return;
            }

            if (heapStart == nullptr)
            {
                return;
            }

            m_commandList->OMSetRenderTargets(targetCnt, heapStart, RTsSingleHandleToDescriptorRange, depthStencilDesctriptor);
        
        }

        void CommandList::SetViewport(UINT32 width, UINT32 height)
        {
            m_viewport.Width = width;
            m_viewport.Height = height;
            m_viewport.TopLeftX = 0;
            m_viewport.TopLeftY = 0;
            m_viewport.MaxDepth = 1.0f;
            m_viewport.MinDepth = 0.0f;
        }
        void CommandList::SetScissorRect(UINT32 width, UINT32 height)
        {
            m_scissorRect.top = 0;
            m_scissorRect.left = 0;
            m_scissorRect.right = m_scissorRect.left + width;
            m_scissorRect.bottom = m_scissorRect.top + height;
        }

        void CommandList::SetRootSignature(ID3D12RootSignature* rootSignature)
        {
            // 初期化していないため、関数を終了させる
            if (m_commandList == nullptr)
            {
                return;
            }

            if (rootSignature == nullptr)
            {
                return;
            }

            m_commandList->SetGraphicsRootSignature(rootSignature);

        }
        void CommandList::ClearScreen(D3D12_CPU_DESCRIPTOR_HANDLE* renderTargetView)
        {
            // 初期化していないため、関数を終了させる
            if (m_commandList == nullptr)
            {
                return;
            }

            if (renderTargetView == nullptr)
            {
                return;
            }

            m_commandList->ClearRenderTargetView(*renderTargetView, CLEAR_COLOR, 0, nullptr);
        }
    }
}
