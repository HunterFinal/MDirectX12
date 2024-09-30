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

#ifndef _M_CMDLIST
#define _M_CMDLIST

#include <d3d12.h>
#include <ComPtr.h>

namespace MFramework
{
    namespace MGraphics_DX12
    {
        class CommandList
        {
            public:
                CommandList();
                ~CommandList();

            public:
                bool Init(ID3D12Device* device, ID3D12CommandAllocator* allocator);

                void SetVertexBuffer(UINT startIndex, UINT vertBufferCnt, D3D12_VERTEX_BUFFER_VIEW* bufferView);
                void SetBarrier(UINT barrierCnt, D3D12_RESOURCE_BARRIER* barrier);
                void SetPipelineState(ID3D12PipelineState* pipelineState);
                void SetRenderTargets(
                                        UINT targetCnt,
                                        D3D12_CPU_DESCRIPTOR_HANDLE* heapStart,
                                        bool RTsSingleHandleToDescriptorRange, 
                                        D3D12_CPU_DESCRIPTOR_HANDLE* depthStencilDesctriptor
                                     );
                void SetViewport(D3D12_VIEWPORT viewport);

            // TODO
            public:
                void ClearScreen(D3D12_CPU_DESCRIPTOR_HANDLE* renderTargetView);

            private:
                CommandList(const CommandList& other) = delete;
                CommandList& operator=(const CommandList& other) = delete;

            private:
                ComPtr<ID3D12GraphicsCommandList> m_commandList;
                D3D12_VIEWPORT m_viewport;
                D3D12_RECT m_scissorRect;
        };
    }
}

#endif