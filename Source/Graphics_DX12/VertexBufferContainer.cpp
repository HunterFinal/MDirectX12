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

#include <Graphics_DX12/VertexBufferContainer.h>

namespace MFramework
{
    namespace MGraphics_DX12
    {
        VertexBufferContainer::VertexBufferContainer()
            : m_vertices()
            , m_vertexBuffer(nullptr)
            , m_vertexBufferView({})
            , m_indices()
            , m_indexBuffer(nullptr)
            , m_indexBufferView({})
            , m_isCreated(false)
        {

        }
        VertexBufferContainer::~VertexBufferContainer()
        {
            Dispose();
        }
        bool VertexBufferContainer::CreateVertexBuffer(ID3D12Device* device, DirectX::XMFLOAT3* verts, UINT32 vertCnt, UINT32* indices, UINT32 indexCnt)
        {
            if (device == nullptr)
            {
                return false;
            }
            if ((verts == nullptr) || (indices == nullptr))
            {
                return false;
            }

            // 前のデータが入っていると削除する
            if (m_isCreated)
            {
                Dispose();
            }
            
            m_vertices.resize(vertCnt);
            for (int i = 0u; i < vertCnt; ++i)
            {
                m_vertices.push_back(verts[i]);
            }

            m_indices.resize(indexCnt);
            for (int i = 0u; i < indexCnt; ++i)
            {
                m_indices.push_back(m_indices[i]);
            }

            HRESULT result = S_OK;
            // 頂点バッファー作成
            {
                D3D12_HEAP_PROPERTIES heapProperties = {};

                heapProperties.Type                 = D3D12_HEAP_TYPE_UPLOAD;
                heapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
                heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

                // 頂点バッファー情報
                D3D12_RESOURCE_DESC resourceDesc = {};

                resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;       // バッファーに使うためBUFFERを指定
                resourceDesc.Width = sizeof(DirectX::XMFLOAT3) * vertCnt;       // 頂点情報が入るだけのサイズ
                resourceDesc.Height = 1;                                        // 幅で表現しているので１とする
                resourceDesc.DepthOrArraySize = 1;                              // 1でよい
                resourceDesc.MipLevels = 1;                                     // 1でよい
                resourceDesc.Format = DXGI_FORMAT_UNKNOWN;                      // 今回は画像ではないためUNKNOWNでよい
                resourceDesc.SampleDesc.Count = 1;                              // アンチエイリアシングを行うときのパラメーター。※ 0だとデータがないことになってしまうため、1にする
                resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;           // UNKNOWNと指定すると、自動で最適なレイアウトに設定しようとする　※今回はテクスチャではないため不適切です
                resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;                  // NONEでよい※要調べ

                result = device->CreateCommittedResource(
                                                            &heapProperties,
                                                            D3D12_HEAP_FLAG_NONE,
                                                            &resourceDesc,
                                                            D3D12_RESOURCE_STATE_GENERIC_READ,
                                                            nullptr,
                                                            IID_PPV_ARGS(m_vertexBuffer.GetAddressOf())
                                                        );

                // 作成失敗
                if (FAILED(result))
                {
                    Dispose();
                    return false;
                }

                DirectX::XMFLOAT3* _vertMap = nullptr;
                // ID3D12Resource::Mapメソッド
                // 第一引数: ミップマップなどではないため0でよい　  ※リソース配列やミップマップの場合、サブリソース番号を渡す
                // 第二引数: 範囲指定。全範囲なのでnullptrでよい   ※一部のみを更新したいときは設定する
                // 第三引数: 受け取るためのポインター変数のアドレス

                result = m_vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_vertMap));
                if(FAILED(result))
                {
                    Dispose();
                    return false;
                }

                std::copy(std::begin(m_vertices), std::end(m_vertices), _vertMap);

                m_vertexBuffer->Unmap(0, nullptr);
            }

            return true;
        }

        void VertexBufferContainer::Dispose()
        {
            m_vertices.clear();
            m_vertices.shrink_to_fit();
            m_vertexBuffer.Reset();
            memset(&m_vertexBufferView, 0, sizeof(m_vertexBufferView));

            m_indices.clear();
            m_indices.shrink_to_fit();
            m_indexBuffer.Reset();
            memset(&m_indexBufferView, 0, sizeof(m_indexBufferView));

            m_isCreated = false;
        }
    }
}