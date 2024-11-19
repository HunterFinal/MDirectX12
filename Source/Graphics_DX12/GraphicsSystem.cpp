/*

MRenderFramework
Author : MAI ZHICONG

Description : DirectX12 Graphics System (Graphics API: DirectX12)

Update History: 2024/11/13 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <Graphics_DX12/GraphicsSystem.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(lib, "DirectXTex.lib")

#include <FileUtil.h> 
#include <D3D12EasyUtil.h>
#include <string>
#include <cassert>

struct Vertex
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 uv;
};

static float angle;
static DirectX::XMMATRIX worldMatrix;
static DirectX::XMMATRIX viewMatrix;
static DirectX::XMFLOAT3 eye(0,0,-5), target(0,0,0), up(0,1,0);
static DirectX::XMMATRIX projectionMatrix;
static DirectX::XMMATRIX transformMatrix;

namespace
{
  constexpr size_t FRAME_COUNT = 2;
  constexpr D3D12_COMMAND_LIST_TYPE CMD_LIST_TYPE = D3D12_COMMAND_LIST_TYPE_DIRECT;
  constexpr D3D12_FILTER FILTER = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // 線形補間
  const Color DEFAULT_SKYBOX_COLOR = Color::black; 

}

namespace MFramework
{
  GraphicsSystem::GraphicsSystem()
    : m_dxgiFactory()
    , m_device()
    , m_cmdList()
    , m_cmdQueue()
    , m_swapChain()
    , m_rtvHeap()
    , m_texHeap()
    , m_renderTargets()
    , m_fence()
    , m_constBuffer()
    , m_vertBuffer()
    , m_idxBuffer()
    , m_vertShader()
    , m_pixelShader()
    , m_rootSig()
    , m_pipelineState()
    , m_texture() 
    , m_viewPort({})
    , m_scissorRect({})
    , m_clearColor(DEFAULT_SKYBOX_COLOR)
    , m_debugDevice(nullptr)
  { }

  GraphicsSystem::~GraphicsSystem()
  {
    Terminate();
  }

  void GraphicsSystem::Init(HWND hWnd, bool isDebugMode)
  {
    // Debug version assertion
    assert(hWnd != nullptr);

    // Release version null check
    if (hWnd == nullptr)
    {
      terminate();
    }

    HRESULT result = S_OK;

    if (isDebugMode)
    {
      #ifdef _DEBUG
        // メモリリークを知らせる
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
        
        ComPtr<ID3D12Debug> debugLayer = nullptr;
        result = D3D12GetDebugInterface(IID_PPV_ARGS(debugLayer.ReleaseAndGetAddressOf()));
        if (SUCCEEDED(result))
        {
          debugLayer->EnableDebugLayer();
          debugLayer->Release();
          debugLayer.Reset();
        }
      #else
        {
          // Do something later
        }
      #endif

      RECT wndRect;
      GetClientRect(hWnd, &wndRect);
      const LONG wndWidthL = wndRect.right - wndRect.left;
      const LONG wndHeightL = wndRect.bottom - wndRect.top;
      const float wndWidthF = static_cast<float>(wndWidthL);
      const float wndHeightF = static_cast<float>(wndHeightL);
      const float wndAspect = wndWidthF / wndHeightF;

      // DX12初期化
      m_dxgiFactory.Init();
      m_device.Init(m_dxgiFactory.Get());
      #ifdef _DEBUG
        if (isDebugMode)
        {
          HRESULT result = m_device.Get()->QueryInterface(m_debugDevice.ReleaseAndGetAddressOf());
        }
      #else
      {
        // Maybe do something
      }
      #endif
      m_cmdList.Init(m_device.Get(), CMD_LIST_TYPE, FRAME_COUNT);
      m_cmdQueue.Init(m_device.Get(), CMD_LIST_TYPE);
      m_swapChain.Init(m_dxgiFactory.Get(), m_cmdQueue.Get(), hWnd, FRAME_COUNT);
      m_rtvHeap.Init(m_device.Get(), D3D12DescHeapType::RTV, FRAME_COUNT);

      m_renderTargets.resize(FRAME_COUNT);
      for (size_t i = 0; i < FRAME_COUNT; ++i)
      {
        auto handle = m_rtvHeap.GetHandle(i);
        m_renderTargets[i].Create(m_device.Get(), m_swapChain.Get(), i, &handle);
      }

      m_fence.Init(m_device.Get());

      // TODO magic number
      m_texHeap.Init(m_device.Get(), D3D12DescHeapType::CBV_SRV_UAV, 2);

      assert(m_texture.Create( m_device.Get(),
                              &m_cmdList,
                              m_cmdQueue.Get(),
                              m_texHeap.GetHandle(0),
                              L"textest.png"
                            ));

      // 定数バッファー作成
      worldMatrix = DirectX::XMMatrixRotationY(DirectX::XM_PIDIV4);

      // ウインドウ左上が(-1,1),右下が(1,-1)のため
      // 座標変換行列は
      // |2/window.width,               0, 0, -1|
      // |             0,-2/window.height, 0,  1|
      // |             0,               0, 1,  0|
      // |             0,               0, 0,  1|

      // matrix.r[0].m128_f32[0] = 2.0f / test.GetWidth();
      // matrix.r[1].m128_f32[1] = -2.0f / test.GetHeight();
      // // 行優先のため rは行を表している
      // matrix.r[3].m128_f32[0] = -1.0f;
      // matrix.r[3].m128_f32[1] = 1.0f;
      viewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&eye), DirectX::XMLoadFloat3(&target), DirectX::XMLoadFloat3(&up));
      projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
                                                            DirectX::XM_PIDIV2,  // 画角が90° Pi/2
                                                            wndAspect,           // アスペクト比
                                                            1.0f,                                 // 近接クリップ面
                                                            10.0f                                 // 遠方クリップ面  
                                                          );
      // 行優先のため変換行列は world * view * projection
      transformMatrix = worldMatrix * viewMatrix * projectionMatrix;

      // TODO magic number;
      MFramework::DescriptorHandle constHandle = m_texHeap.GetHandle(1);
      m_constBuffer.Create(m_device.Get(), constHandle, 1, &transformMatrix);

      // TODO 
      {
        // 頂点座標
        // ※時計回り
        Vertex vertices[] = 
        {
            {{   -1.0f, -1.0f, 0.0f},{0.0f, 1.0f}},     // 左下
            {{   -1.0f,  1.0f, 0.0f},{0.0f, 0.0f}},     // 左上
            {{    1.0f, -1.0f, 0.0f},{1.0f, 1.0f}},     // 右下
            {{    1.0f,  1.0f, 0.0f},{1.0f, 0.0f}},     // 右上
        };
        // 頂点インデックス作成
        UINT16 indices[] = {
                              0, 1, 2, 
                              2, 1, 3 
                            };
        m_vertBuffer.Create(m_device.Get(), sizeof(vertices), vertices);
        m_idxBuffer.Create(m_device.Get(), sizeof(indices), indices);
      }

      // 頂点バッファービューを設定
      // 第一引数:※頂点バッファーの設定を開始するには、デバイスの 0 から始まる配列にインデックスを作成します。
      // 第二引数:渡す頂点バッファービューの数
      // 第三引数:頂点バッファービューの配列の先頭アドレス
      D3D12_VERTEX_BUFFER_VIEW vertView = m_vertBuffer.GetView();
      m_cmdList->IASetVertexBuffers(0, 1, &vertView);

      // TODO   
      m_cmdList->Close();

      // 頂点シェーダー作成    
      if (!m_vertShader.InitFromCSO(L"BasicVertexShader.cso"))
      {
        std::wstring vertShaderHLSLPath = L"Shaders/HLSLs/BasicVertexShader.hlsl";
        if (!m_vertShader.InitFromFile(vertShaderHLSLPath.c_str(), "BasicVS", "vs_5_1"))
        {
          // TODO
          assert(false);//bad design;
        }
      }
      // ピクセルシェーダー作成
      if (!m_pixelShader.InitFromCSO(L"BasicPixelShader.cso"))
      {
        std::wstring pixelShaderHLSLPath = L"Shaders/HLSLs/BasicPixelShader.hlsl";
        if (!m_pixelShader.InitFromFile(pixelShaderHLSLPath.c_str(), "BasicPS", "ps_5_1"))
        {
          // TODO
          assert(false);//bad design;
        }
      }

      // ルートシグネチャー作成
      m_rootSig.Init(m_device.Get(), FILTER);
       // パイプラインステート設定
      m_pipelineState.Init(m_device.Get(), m_rootSig.Get(), &m_vertShader, &m_pixelShader, nullptr);

      // ビューポートを作成
      m_viewPort.Width = wndWidthF;
      m_viewPort.Height = wndHeightF;
      m_viewPort.TopLeftX = 0;
      m_viewPort.TopLeftY = 0;
      m_viewPort.MaxDepth = 1.0f;
      m_viewPort.MinDepth = 0.0f;

      // シザー矩形
      // ビューポートに出力された画像のどこからどこまでを実際に画面に映し出すかを設定するためのもの

      m_scissorRect.top = 0;
      m_scissorRect.left = 0;
      m_scissorRect.right = m_scissorRect.left + wndWidthL;
      m_scissorRect.bottom = m_scissorRect.top + wndHeightL;
      
    }
  }

  void GraphicsSystem::PreProcess()
  {
    // レンダーターゲットビューのインデックス取得
    UINT backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
    // リセットし、命令オブジェクトをためていく
    // コマンドリストのクローズ状態を解除
    m_cmdList.Reset(static_cast<int>(backBufferIndex), m_pipelineState.Get());

    {
      // リソースバリアを設定
      D3D12_RESOURCE_BARRIER barrierDesc = {};

      barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;                                // 遷移
      barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;                                     // 特に指定なし
      barrierDesc.Transition.pResource = m_renderTargets[backBufferIndex].Get();                  // バックバッファーリソース
      barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;             // サブリソース番号
      barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;                        // 直前はPRESENT状態
      barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;                   // 今からレンダーターゲット状態

      // バリア指定実行
      // 第一引数:設定バリアの数(現時点では1でよい)
      // 第二引数:設定バリア構造体アドレス(配列で同時に設定できる)
      m_cmdList->ResourceBarrier(1, &barrierDesc);
    }
    // レンダーターゲットを指定
    auto rtvHandle = m_rtvHeap.GetHandle(backBufferIndex);

    // レンダーターゲットビューをセット 
    // 第一引数:レンダーターゲット数(今回は一つだけで1でよい)
    // 第二引数:レンダーターゲットハンドル先頭アドレス
    // TODO 第三引数:複数時に連続しているか
    // 第四引数:深度ステンシルバッファービューのハンドル(nullptrでよい)
    if (rtvHandle.HasCPUHandle())
    {
      m_cmdList->OMSetRenderTargets(1, &rtvHandle.CPUHandle, false, nullptr);
      // 画面クリア
      
      // TODO
      float clearColor[] = {m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a};
      m_cmdList->ClearRenderTargetView(rtvHandle.CPUHandle, clearColor, 0, nullptr);
    }
  }

  void GraphicsSystem::Render()
  {
    // TODO
    angle += 0.03f;
    worldMatrix = DirectX::XMMatrixRotationY(angle);
    transformMatrix = worldMatrix * viewMatrix * projectionMatrix;

    m_constBuffer.Remap(1, &transformMatrix);

    ID3D12DescriptorHeap* pHeaps[] = 
    {
      m_texHeap.Get(),
    };
    // ルートシグネチャー設定
    m_cmdList->SetGraphicsRootSignature(m_rootSig.Get());
    m_cmdList->SetDescriptorHeaps(_countof(pHeaps), pHeaps);
    m_cmdList->RSSetViewports(1, &m_viewPort);
    m_cmdList->RSSetScissorRects(1, &m_scissorRect);

    // パイプラインステートを設定
    m_cmdList->SetPipelineState(m_pipelineState.Get()); 

    m_cmdList->SetGraphicsRootDescriptorTable(
                                                0,           // ルートパラメーターインデックス 
                                                m_texture.GetGPUHandle()   // ヒープアドレス
                                              );
  
    // 頂点バッファーを設定
    // 第一引数:スロット番号
    // 第二引数:頂点バッファービューの数
    // 第三引数:頂点バッファービューの配列
    D3D12_VERTEX_BUFFER_VIEW vertView = m_vertBuffer.GetView();
    D3D12_INDEX_BUFFER_VIEW idxView = m_idxBuffer.GetView();
    m_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_cmdList->IASetVertexBuffers(0, 1, &vertView);
    m_cmdList->IASetIndexBuffer(&idxView);  
    // 描画命令を設定
    // 第一引数:頂点インデックス数
    // 第二引数:インスタンス数
    // 第三引数:頂点データのオフセット
    // 第四引数:インスタンスのオフセット
    // ※インスタンス数は同じプリミティブをいくつ表示するかという意味です
    m_cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
  }

  void GraphicsSystem::PostProcess()
  {
    // レンダーターゲットビューのインデックス取得
    UINT backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
    {
      // リソースバリアを設定
      D3D12_RESOURCE_BARRIER barrierDesc = {};

      barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;                                // 遷移
      barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;                                     // 特に指定なし
      barrierDesc.Transition.pResource = m_renderTargets[backBufferIndex].Get();                  // バックバッファーリソース
      barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;             // サブリソース番号
      barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;    // 直前はレンダーターゲット状態
      barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;           // 今からPRESENT状態
      // バリア指定実行
      // 第一引数:設定バリアの数(現時点では1でよい)
      // 第二引数:設定バリア構造体アドレス(配列で同時に設定できる)
      m_cmdList->ResourceBarrier(1, &barrierDesc);
    }

    // ためておいた命令を実行
    // その前に命令をクローズが必須
    m_cmdList->Close();
    // 命令の実行
    ID3D12CommandList* cmdLists[] = { m_cmdList.Get() };
    // 第一引数:実行するコマンドリストの数(1でよい)
    // 第二引数:コマンドリスト配列の先頭アドレス
    m_cmdQueue.Execute(1, cmdLists);

    // フェンスを使ってGPUの処理が終わるまで待つ
    m_fence.Wait(m_cmdQueue.Get());

    // フリップ
    // 第一引数:フリップまでの待ちフレーム数
    // ※0にするとPresentメソッドが即時復帰して次のフレームが始まってしまいます。
    // ※1にすると垂直同期を待つ
    // 第二引数:さまざまな指定を行います
    // テスト用出力やステレオをモノラル表示など特殊な用途であるため、今回は0にする
    m_swapChain->Present(1, 0);
  }

  void GraphicsSystem::Terminate() noexcept
  {
    m_dxgiFactory.Dispose();
    m_cmdList.Dispose();
    m_cmdQueue.Dispose();
    m_swapChain.Dispose();
    m_rtvHeap.Dispose();
    m_texHeap.Dispose();

    for (size_t i = 0; i < m_renderTargets.size(); ++i)
    {
      m_renderTargets[i].Dispose();
    }

    m_renderTargets.clear();
    m_renderTargets.shrink_to_fit();
    m_fence.Dispose();
    m_constBuffer.Dispose();
    m_vertBuffer.Dispose();
    m_idxBuffer.Dispose();
    m_vertShader.Dispose();
    m_pixelShader.Dispose();
    m_rootSig.Dispose();
    m_pipelineState.Dispose();
    m_texture.Dispose();
    m_device.Dispose();
    
    if (m_debugDevice.Get() != nullptr)
    {
      m_debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
      m_debugDevice.Reset();
    }

  }
}