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

  /// @brief
  /// アライメントに揃えたサイズを返す
  /// @param size 元のサイズ
  /// @param alignment アラインメントサイズ
  /// @return アラインメントを揃えたサイズ
  size_t AlignmentedSize(size_t size, size_t alignment)
  {
    return size + alignment - size % alignment;
  }

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
    , m_viewPort({})
    , m_scissorRect({})
    , m_clearColor(DEFAULT_SKYBOX_COLOR)
  { }

  GraphicsSystem::~GraphicsSystem()
  {
    Terminate();
  }

  void GraphicsSystem::Init(HWND hWnd, bool isDebugMode)
  {
    assert(hWnd != nullptr);

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
      const float wndWidth = static_cast<float>(wndRect.right - wndRect.left);
      const float wndHeight = static_cast<float>(wndRect.bottom - wndRect.top);
      const float wndAspect = wndWidth / wndHeight;


      // DX12初期化
      m_dxgiFactory.Init();
      m_device.Init(m_dxgiFactory.Get());
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

      #pragma region need refactoring
      // テクスチャ
      // WICテクスチャのロード
      DirectX::TexMetadata metadata = {};
      DirectX::ScratchImage scratchImg = {};
      // テクスチャバッファー作成
      ID3D12Resource* texBuffer = nullptr;
      {

        result = DirectX::LoadFromWICFile(
                                          L"Assets/Images/textest.png",    // ファイルパス
                                          DirectX::WIC_FLAGS_NONE,         // どのようにロードするかを示すフラグ
                                          &metadata,                       // メタデータ(DirectX::TexMetadata)を受け取るためのポインター
                                          scratchImg                       // 実際のデータが入っているオブジェクト
                                        ); 
        assert(SUCCEEDED(result));

        const DirectX::Image* img = scratchImg.GetImage(0, 0, 0);  // 生のデータ抽出

        // 中間バッファーとしてのアップロードヒープ設定
        D3D12_HEAP_PROPERTIES uploadHeapProp = {};

        // マップ可能にするため、UPLOADにする
        uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

        // アップロード用に使用すること前提なのでUNKNOWNでよい
        uploadHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        uploadHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        // 単一アダプターのため 0
        uploadHeapProp.CreationNodeMask = 0;
        uploadHeapProp.VisibleNodeMask = 0;

        D3D12_RESOURCE_DESC resDesc = {};

        // 単なるデータの塊なのでUNKNOWN
        resDesc.Format = DXGI_FORMAT_UNKNOWN;
        // 単なるバッファーとして指定
        resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

        // rowPitchが変わったためバッファーのサイズも変更しなければならない
        // アラインメントされたサイズ*高さとする必要がある
        resDesc.Width = AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * img->height;    // データサイズ
        resDesc.Height = 1;
        resDesc.DepthOrArraySize = 1;
        resDesc.MipLevels = 1;

        resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;  // 連続したデータ
        resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;         // 特にフラグなし

        resDesc.SampleDesc.Count = 1; //通常テクスチャなのでアンチエイリアシングしない
        resDesc.SampleDesc.Quality = 0;

        // 中間バッファー作成
        ID3D12Resource* uploadBuffer = nullptr;

        result = m_device->CreateCommittedResource(
                                                  &uploadHeapProp,
                                                  D3D12_HEAP_FLAG_NONE, //特にフラグなし
                                                  &resDesc,
                                                  D3D12_RESOURCE_STATE_GENERIC_READ,    // CPUから書き込み可能だが、GPUから見ると読み取り専用
                                                  nullptr,
                                                  IID_PPV_ARGS(&uploadBuffer)
                                                );     

        assert(SUCCEEDED(result));

        // テクスチャのためのヒープ設定
        D3D12_HEAP_PROPERTIES texHeapProp = {};

        texHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT; // テクスチャ用
        texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        // 単一アダプターのため 0
        texHeapProp.CreationNodeMask = 0;
        texHeapProp.VisibleNodeMask = 0;

        // リソース設定 (変数使いまわし)
        resDesc.Format = metadata.format;
        resDesc.Width = static_cast<UINT64>(metadata.width);
        resDesc.Height = static_cast<UINT>(metadata.height);
        resDesc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);    // 2D配列でもないので1
        resDesc.MipLevels = static_cast<UINT16>(metadata.mipLevels);           // ミップマップしないためミップ数は1つ
        resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
        resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

        result = m_device->CreateCommittedResource(
                                                  &texHeapProp,
                                                  D3D12_HEAP_FLAG_NONE,
                                                  &resDesc,
                                                  D3D12_RESOURCE_STATE_COPY_DEST, // コピー先
                                                  nullptr,
                                                  IID_PPV_ARGS(&texBuffer)
                                                );

        assert(SUCCEEDED(result));

        // アップロードリソースへのマップ
        UINT8* mapforImg = nullptr; // img->pixelsと同じ型にする
        result = uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mapforImg));

        // 元データをコピーする際に、元データのRowPitchとバッファーのRowPitchが合わないため、
        // 一行ごとにコピーして行頭が合うようにする
        UINT8* srcAddress = img->pixels;

        size_t rowPitch = AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

        for (int y = 0; y < img->height; ++y)
        {
          std::copy_n(srcAddress, rowPitch, mapforImg);

          // 一行ごとの辻褄を合わせる
          srcAddress += img->rowPitch;
          mapforImg += rowPitch;
        }

        uploadBuffer->Unmap(0, nullptr); //アンマップ

        D3D12_TEXTURE_COPY_LOCATION src = {};

        // コピー元(アップロード側)設定
        src.pResource = uploadBuffer;   // 中間バッファー
        // D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINTの場合、pResourceはバッファーリソースを指す必要がある
        // D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEXの場合、pResourceはテクスチャリソースを指す必要がある
        src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;  // フットプリント指定
        src.PlacedFootprint.Offset = 0;
        src.PlacedFootprint.Footprint.Width = static_cast<UINT>(metadata.width);
        src.PlacedFootprint.Footprint.Height = static_cast<UINT>(metadata.height);
        src.PlacedFootprint.Footprint.Depth = static_cast<UINT>(metadata.depth);
        // ここが重要
        // RowPitchが256の倍数でないとCopyTextureRegionの実行は失敗する
        // 256アライメントにする
        src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));
        src.PlacedFootprint.Footprint.Format = img->format;

        D3D12_TEXTURE_COPY_LOCATION dst = {};

        // コピー先設定
        dst.pResource = texBuffer;
        dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dst.SubresourceIndex = 0;

        {
          m_cmdList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

          D3D12_RESOURCE_BARRIER barrierDesc = {};

          barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
          barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
          barrierDesc.Transition.pResource = texBuffer;
          barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
          barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;    // ここが重要
          barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // ここが重要

          m_cmdList->ResourceBarrier(1, &barrierDesc);
          m_cmdList->Close();

          // コマンドリストの実行
          ID3D12CommandList* cmdLists[] = { m_cmdList.Get() };
          m_cmdQueue.Execute(1, cmdLists);

          m_fence.Wait(m_cmdQueue.Get());

          m_cmdList.Reset(0);
        }

      }
      #pragma endregion needrefactoring

      // TODO magic number
      m_texHeap.Init(m_device.Get(), D3D12DescHeapType::CBV_SRV_UAV, 2);

        // シェーダーリソースビューを作成
      D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

      srvDesc.Format = metadata.format;                                           // RGBA(0.0f~1.0fに正規化)
      srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;                      // 2Dテクスチャ
      srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; // データのRGBAをどのようにマッピングするかということを指定するためのもの
                                                                                  // 画像の情報がそのままRGBAなど「指定されたフォーマットに、
                                                                                  // データどおりの順序で割り当てられている」ことを表しています。
      srvDesc.Texture2D.MipLevels = 1;                                            // ミップマップは使用しないので１

      // 三つ目の引数はディスクリプタヒープのどこにこのビューを配置するかを指定するためのもの
      // 複数のテクスチャビューがある場合、取得したハンドルからオフセットを指定する必要がある
      
      MFramework::DescriptorHandle texHeapHandle = m_texHeap.GetHandle(0);

      if (texHeapHandle.HasCPUHandle())
      {
        m_device->CreateShaderResourceView(
                                            texBuffer,                                          // ビューと関連付けるバッファー
                                            &srvDesc,                                           // 先ほど設定したテクスチャ設定情報
                                            texHeapHandle.CPUHandle                           // ヒープのどこに割り当てるか
                                          );
      }

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
      m_viewPort.Width = wndWidth;
      m_viewPort.Height = wndHeight;
      m_viewPort.TopLeftX = 0;
      m_viewPort.TopLeftY = 0;
      m_viewPort.MaxDepth = 1.0f;
      m_viewPort.MinDepth = 0.0f;

      // シザー矩形
      // ビューポートに出力された画像のどこからどこまでを実際に画面に映し出すかを設定するためのもの

      m_scissorRect.top = 0;
      m_scissorRect.left = 0;
      m_scissorRect.right = m_scissorRect.left + static_cast<LONG>(wndWidth);
      m_scissorRect.bottom = m_scissorRect.top + static_cast<LONG>(wndHeight);
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

    auto texHeapHandle = m_texHeap.GetHandle(0);
    if (texHeapHandle.HasGPUHandle())
    {
      m_cmdList->SetGraphicsRootDescriptorTable(
                                                0,           // ルートパラメーターインデックス 
                                                texHeapHandle.GPUHandle   // ヒープアドレス
                                              );
    }
  
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

    // リセットし、命令オブジェクトをためていく
    // コマンドリストのクローズ状態を解除
    m_cmdList.Reset(static_cast<int>(backBufferIndex), m_pipelineState.Get());

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
    m_device.Dispose();
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
  }
}