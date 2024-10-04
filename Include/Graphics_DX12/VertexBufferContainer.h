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

#ifndef M_VERTBUFFER_CONTAINER
#define M_VERTBUFFER_CONTAINER 1

#include <d3d12.h>
#include <DirectXMath.h>

#include <vector>

// Useful Utilities
#include <ComPtr.h>
#include <Interfaces/IDisposable.h>

namespace MFramework
{
    namespace MGraphics_DX12
    {
        class VertexBufferContainer : public MFramework::IDisposable
        {
            using Ref = VertexBufferContainer&;
            using Const_Ref = const Ref;

            public:
                VertexBufferContainer();
                ~VertexBufferContainer();

            private:
                VertexBufferContainer(Const_Ref other) = delete;
                Ref operator=(Const_Ref other) = delete;

            public:
                bool CreateVertexBuffer(ID3D12Device* device, DirectX::XMFLOAT3* verts, UINT32 vertCnt, UINT32* indices, UINT32 indexCnt);

            public:
                void Dispose(void) override;

            private:
                std::vector<DirectX::XMFLOAT3> m_vertices;
                ComPtr<ID3D12Resource> m_vertexBuffer;
                D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

                std::vector<UINT32> m_indices;
                ComPtr<ID3D12Resource> m_indexBuffer;
                D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

                bool m_isCreated;
        };

    }
}
#endif