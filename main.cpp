/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderFramework used by Game (Graphics API: DirectX12)

Update History: 2024/09/19 Create

Version : alpha_1.0.0

*/

// DirectX 12、シェーダー モデル 5.1、D3DCompile API、FXC はすべて非推奨です。 
// 代わりに、DXIL 経由でシェーダー モデル 6 を使用してください。 GitHub を参照してください。
// url https://github.com/microsoft/DirectXShaderCompiler

// Debug include
#ifdef _DEBUG

#include <DebugHelper>

#endif
// end Debug include

// MWindow include
#include <Window/BaseWindow.h>
#include <Interfaces/IWindowInfo.h>
// end MWindow include

// DX12 include
#include <d3d12.h>
#include <dxgi1_6.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <DirectXMath.h>

// end DX12 include

// shader compiler include
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

// end shader compiler include

// DirectXTex lib include

#include <DirectXTex.h>

#pragma comment(lib, "DirectXTex.lib")

// end DirectXTex lib include

#include <vector>
#include <string>

#pragma region DX12 Wrapper

#include <Graphics_DX12/DX12Device.h>
#include <Graphics_DX12/DX12DXGIFactory.h>
#include <Graphics_DX12/CommandList.h>
#include <Graphics_DX12/CommandQueue.h>
#include <Graphics_DX12/RootSignature.h>
#include <Graphics_DX12/DX12SwapChain.h>
#include <Graphics_DX12/Fence.h>

#include <Graphics_DX12/VertexBufferContainer.h>
#include <Graphics_DX12/IndexBufferContainer.h>

#include <Graphics_DX12/DescriptorHandle.h>
#include <Graphics_DX12/ConstantBuffer.h>

#include <Graphics_DX12/ShaderResBlob.h>

#pragma endregion
// end region of DX12 Wrapper

#include <Graphics_DX12/Texture.h>
#include <RandomGenerator.hpp>

struct Vertex
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 uv;
};

/// @brief
/// アライメントに揃えたサイズを返す
/// @param size 元のサイズ
/// @param alignment アラインメントサイズ
/// @return アラインメントを揃えたサイズ
size_t AlignmentedSize(size_t size, size_t alignment)
{
  return size + alignment - size % alignment;
}

#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
#endif
{
    #pragma region D3D12 DebugLayer

    #ifdef _DEBUG
        // メモリリークを知らせる
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
        
        ID3D12Debug* _debugLayer = nullptr;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&_debugLayer))))
        {
            _debugLayer->EnableDebugLayer();
            _debugLayer->Release();
        }
    #endif

    #pragma endregion
    // end region of D3D12 DebugLayer

    // Window
    MWindow::Window test;

    if(!test.InitWnd(1920, 1080, L"GameWindow", L"Framework_Window_Title"))
    {
        return -1;
    }

    HRESULT result = S_OK;

    // アダプターを列挙するためDXGIFactoryオブジェクトを作成 
    MFramework::DX12DXGIFactory dxgiFactory;
    dxgiFactory.Init();

    MFramework::DX12Device device;
    device.Init(dxgiFactory.Get());

    #pragma region D3D12 CommandAllocator and CommandList
    // コマンドリストの作成とコマンドアロケーター

    const auto CMD_LIST_TYPE = D3D12_COMMAND_LIST_TYPE_DIRECT;
    const int FRAME_COUNT = 2;
    // 実際に命令をためているインターフェース

    // 命令のインターフェース
    MFramework::CommandList cmdList;
    cmdList.Init(device.Get(), CMD_LIST_TYPE, FRAME_COUNT);

    #pragma endregion
    // end region of D3D12 CommandAllocator and CommandList

    #pragma region D3D12 CommandQueue
    // コマンドキュー
    // コマンドリストを用いてためた命令セットをGPUが順次実行していくためのもの
    MFramework::CommandQueue cmdQueue;
    cmdQueue.Init(device.Get(), CMD_LIST_TYPE);

    #pragma endregion
    // end region of D3D12 CommandQueue

    #pragma region D3D12 SwapChain for Double buffering
    // スワップチェーンオブジェクト
    MFramework::SwapChain swapChain;
    swapChain.Init(dxgiFactory.Get(), cmdQueue.Get(), static_cast<MWindow::IWindowInfo*>(&test), FRAME_COUNT);

    #pragma endregion
    // end region of D3D12 SwapChain for Double buffering

    #pragma region D3D12 Descriptor Heap for multiple Descriptors
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};

    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;     // レンダーターゲットビューなのでRTV
    heapDesc.NodeMask = 0;                              // GPUを一つだけ使用する想定なので0を入れる(複数のGPUがある場合に識別を行うためのビットフラグ)
    heapDesc.NumDescriptors = FRAME_COUNT;              // 裏表2つ
    // TODO
    // テクスチャバッファー（SRV）や定数バッファー（CBV）であれば D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;   // 特に指定なし

    ID3D12DescriptorHeap* rtvHeaps = nullptr;

    result = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));

    // 生成失敗
    if(result != S_OK)
    {
        return -1;
    }
    #pragma endregion
    // end region of D3D12 Descriptor Heap for multiple Descriptors

    #pragma region D3D12 SwapChain and Descriptor binding
    // Swap ChainのメソッドGetDescでDXGI_SWAP_CHAIN_DESC取得
    DXGI_SWAP_CHAIN_DESC swcDesc = {};

    result = swapChain->GetDesc(&swcDesc);

    // 取得失敗
    if(result != S_OK)
    {
        return -1;
    }
    
    std::vector<ID3D12Resource*> _backBuffers(swcDesc.BufferCount);

    // SRGBレンダーターゲットビュー設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // ガンマ補正あり (sRGB)
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    // Get DescriptorHeap handle
    D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();

    for(size_t i = 0; i < swcDesc.BufferCount; ++i)
    {
        result = swapChain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(&_backBuffers[i]));
        device->CreateRenderTargetView(_backBuffers[i], &rtvDesc, handle);

        // ハンドルサイズを計算し、先頭からずらす
        handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }
    #pragma endregion
    // end region of D3D12 SwapChain and Descriptor binding


    // GPU側の処理が完了したかどうかを知るための仕組み
    MFramework::Fence fence;
    fence.Init(device.Get());

    // D3D12に使う定数
    // RTVのインクリメントサイズ
    const UINT RTV_INCREMENT_SIZE = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


    
    // テクスチャ
    // WICテクスチャのロード
    DirectX::TexMetadata metadata = {};
    DirectX::ScratchImage scratchImg = {};

    result = DirectX::LoadFromWICFile(
                                       L"Assets/Images/textest.png",    // ファイルパス
                                       DirectX::WIC_FLAGS_NONE,         // どのようにロードするかを示すフラグ
                                       &metadata,                       // メタデータ(DirectX::TexMetadata)を受け取るためのポインター
                                       scratchImg                       // 実際のデータが入っているオブジェクト
                                     ); 
    if (FAILED(result))
    {
      return -1;
    }

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

    result = device->CreateCommittedResource(
                                              &uploadHeapProp,
                                              D3D12_HEAP_FLAG_NONE, //特にフラグなし
                                              &resDesc,
                                              D3D12_RESOURCE_STATE_GENERIC_READ,    // CPUから書き込み可能だが、GPUから見ると読み取り専用
                                              nullptr,
                                              IID_PPV_ARGS(&uploadBuffer)
                                            );     

    if (FAILED(result))
    {
        return -1;
    }

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

    // テクスチャバッファー作成
    ID3D12Resource* texBuffer = nullptr;

    result = device->CreateCommittedResource(
                                              &texHeapProp,
                                              D3D12_HEAP_FLAG_NONE,
                                              &resDesc,
                                              D3D12_RESOURCE_STATE_COPY_DEST, // コピー先
                                              nullptr,
                                              IID_PPV_ARGS(&texBuffer)
                                            );

    if (FAILED(result))
    {
        return -1;
    }

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
      cmdList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

      D3D12_RESOURCE_BARRIER barrierDesc = {};

      barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
      barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
      barrierDesc.Transition.pResource = texBuffer;
      barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
      barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;    // ここが重要
      barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // ここが重要

      cmdList->ResourceBarrier(1, &barrierDesc);
      cmdList->Close();

      // コマンドリストの実行
      ID3D12CommandList* cmdLists[] = { cmdList.Get()};
      cmdQueue.Execute(1, cmdLists);

      fence.Wait(cmdQueue.Get());

      cmdList.Reset(0);
     }

    // テクスチャディスクリプタヒープ作成
    ID3D12DescriptorHeap* basicDescHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};

    // シェーダーから見えるように
    descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    // マスクは0
    descHeapDesc.NodeMask = 0;

    // SRV1つとCBV1つ
    descHeapDesc.NumDescriptors = 2;

    // シェーダーリソースビュー用
    descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

    // 生成
    result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));

    if (FAILED(result))
    {
        return -1;
    }

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
    
    D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();
    device->CreateShaderResourceView(
                                        texBuffer,                                          // ビューと関連付けるバッファー
                                        &srvDesc,                                           // 先ほど設定したテクスチャ設定情報
                                        basicHeapHandle                                     // ヒープのどこに割り当てるか
                                    );
    // 次の場所に移動
    basicHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // 定数バッファー作成
    DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixRotationY(DirectX::XM_PIDIV4);

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
    DirectX::XMFLOAT3 eye(0,0,-5), target(0,0,0), up(0,1,0);
    DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&eye), DirectX::XMLoadFloat3(&target), DirectX::XMLoadFloat3(&up));
    DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
                                                                              DirectX::XM_PIDIV2,  // 画角が90° Pi/2
                                                                              static_cast<float>(test.GetWidth()) 
                                                                            / static_cast<float>(test.GetHeight()), // アスペクト比
                                                                              1.0f,                                 // 近接クリップ面
                                                                              10.0f                                 // 遠方クリップ面  
                                                                          );
    // 行優先のため変換行列は world * view * projection
    DirectX::XMMATRIX transformMatrix = worldMatrix * viewMatrix * projectionMatrix;

    MFramework::ConstantBuffer constBuffer;
    MFramework::DescriptorHandle constHandle;
    constHandle.CPUHandle = basicHeapHandle;
    constBuffer.Create(device.Get(), constHandle, 1, &transformMatrix);

    // 頂点座標
    // ※時計回り
    Vertex vertices[] = 
    {
        {{   -1.0f, -1.0f, 0.0f},{0.0f, 1.0f}},     // 左下
        {{   -1.0f,  1.0f, 0.0f},{0.0f, 0.0f}},     // 左上
        {{    1.0f, -1.0f, 0.0f},{1.0f, 1.0f}},     // 右下
        {{    1.0f,  1.0f, 0.0f},{1.0f, 0.0f}},     // 右上
    };

    // 頂点バッファー作成
    MFramework::VertexBufferContainer vertBuffer;
    vertBuffer.Create(device.Get(), sizeof(vertices), vertices);

    // 頂点インデックス作成
    UINT16 indices[] = {
                            0, 1, 2, 
                            2, 1, 3 
                       };
    
    MFramework::IndexBufferContainer idxBuffer;
    idxBuffer.Create(device.Get(), sizeof(indices), indices);

    // 頂点バッファービューを設定
    // 第一引数:※頂点バッファーの設定を開始するには、デバイスの 0 から始まる配列にインデックスを作成します。
    // 第二引数:渡す頂点バッファービューの数
    // 第三引数:頂点バッファービューの配列の先頭アドレス
    D3D12_VERTEX_BUFFER_VIEW vertView = vertBuffer.GetView();
    cmdList->IASetVertexBuffers(0, 1, &vertView);

    const std::wstring VERTEX_SHADER = L"Shaders/HLSLs/BasicVertexShader.hlsl";
    const std::wstring PIXEL_SHADER = L"Shaders/HLSLs/BasicPixelShader.hlsl";
    // Shaderオブジェクトを作成
    MFramework::ShaderResBlob vertShader;
    MFramework::ShaderResBlob pixelShader;
    // 頂点シェーダー作成
    //vertShader.InitFromFile(VERTEX_SHADER.c_str(), "BasicVS", "vs_5_1");
    vertShader.InitFromCSO(L"BasicVertexShader.cso");
    // ピクセルシェーダー作成
    //pixelShader.InitFromFile(PIXEL_SHADER.c_str(), "BasicPS", "ps_5_1");
    pixelShader.InitFromCSO(L"BasicPixelShader.cso");

    // 頂点レイアウト作成
    // 渡される頂点データなどをどのように解釈するかをGPUに教えてあげるため
    D3D12_INPUT_ELEMENT_DESC _inputLayout[] = 
    {
        // 座標情報
        {
            "POSITION",                                 // セマンティクス名(今回は座標なのでPOSITION)
            0,                                          // 同じセマンティクス名のときに使うインデックス(0でよい) ※要調べ
            DXGI_FORMAT_R32G32B32_FLOAT,                // フォーマット(データの解釈(float3のため,rbg毎に32bit,float型))
            0,                                          // 入力スロットインデックス(0でよい)IASetVertexBuffersでスロットとバッファーとを関連付けている
            D3D12_APPEND_ALIGNED_ELEMENT,               // データが連続していることを表す定義としてALIGNED_ELEMENT
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // データの内容として、1頂点ごとにこのレイアウトが入っているを表す列挙※13.2.2で説明するらしい
            0                                           // 一度に描画するインスタンスの数(インスタンシングを使わないため0でよい)※13.2.2で説明するらしい
        },
        // uv (追加)
        {
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
    };

    // パイプラインステート作成
    D3D12_GRAPHICS_PIPELINE_STATE_DESC _graphicsPipelineDesc = {};

    _graphicsPipelineDesc.pRootSignature = nullptr;
    // シェーダーをセット
    _graphicsPipelineDesc.VS.pShaderBytecode = vertShader.Get()->GetBufferPointer();
    _graphicsPipelineDesc.VS.BytecodeLength = vertShader.Get()->GetBufferSize();
    _graphicsPipelineDesc.PS.pShaderBytecode = pixelShader.Get()->GetBufferPointer();
    _graphicsPipelineDesc.PS.BytecodeLength = pixelShader.Get()->GetBufferSize();

    // サンプルマスクとラスタライザーステート設定
    _graphicsPipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
   
    // ブレンドステート設定
    // DX12の魔導書の範囲においては、マルチサンプリング（アンチエイリアシングの適用）でない場合、AlphaToCoverageEnableの値はαテストの有無を表す（？）
    // マルチサンプリング時の「網羅率」まで考慮して描画を行う
    // IndependentBlendEnableにtrueを指定すれば、合計8個のレンダーターゲットに対して別々にブレンドステート設定できる。falseだったら0番目が全てに適用する。
    _graphicsPipelineDesc.BlendState.AlphaToCoverageEnable = false;
    _graphicsPipelineDesc.BlendState.IndependentBlendEnable = false;

    // BlendOpとLogicOpがあり、メンバーをどう演算するかを表している
    // BlendEnableとLogicOpEnableは演算を行うかを表す変数で、両方同時にtrueになれず、どちらかを選ぶ
    // 今回はブレンドしないため、両方をfalseにする
    D3D12_RENDER_TARGET_BLEND_DESC _renderTargetBlendDesc = {};
    _renderTargetBlendDesc.BlendEnable = false;
    _renderTargetBlendDesc.LogicOpEnable = false;

    // RGBAそれぞれの値を書き込むかどうか指定するための値
    _renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    _graphicsPipelineDesc.BlendState.RenderTarget[0] = _renderTargetBlendDesc;

    // まだアンチエイリアシングを使わないためfalse
    _graphicsPipelineDesc.RasterizerState.MultisampleEnable = false;

    _graphicsPipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;  // カリングしない ※背面カリング、PMDモデルには裏側にポリゴンがないものが多い
    _graphicsPipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // 中身を塗りつぶす ※ポリゴンの中身、ソリッドモデル
    _graphicsPipelineDesc.RasterizerState.DepthClipEnable = true;           // 深度方向のクリッピングは有効に　※後々必要になってくる

    // 残り
    _graphicsPipelineDesc.RasterizerState.FrontCounterClockwise = false;
    _graphicsPipelineDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    _graphicsPipelineDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    _graphicsPipelineDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    _graphicsPipelineDesc.RasterizerState.AntialiasedLineEnable = false;
    _graphicsPipelineDesc.RasterizerState.ForcedSampleCount = 0;
    _graphicsPipelineDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    _graphicsPipelineDesc.DepthStencilState.DepthEnable = false;
    _graphicsPipelineDesc.DepthStencilState.StencilEnable = false;
    
    // 入力レイアウトを設定
    _graphicsPipelineDesc.InputLayout.pInputElementDescs = _inputLayout;
    _graphicsPipelineDesc.InputLayout.NumElements = _countof(_inputLayout); // 配列の数を取得するためのマクロ

    // IBStripCutValueを設定
    // トライアングルストリップのとき、「切り離せない頂点集合」を特定のインデックスで切り離すための指定を行うもの（詳しくはDX12魔導書P138）
    // 今回特に使わない
    _graphicsPipelineDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED; // カットなし

    // PrimitiveTopologyTypeを設定
    // 構成要素が「点」「線」「三角形」のどれなのかを指定
    _graphicsPipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // 設定したレンダーターゲットの数だけフォーマットを指定
    _graphicsPipelineDesc.NumRenderTargets = 1;                             // 今回レンダーターゲットは一つしかないので1
    _graphicsPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;  // 0~1に正規化されたRGBA

    // アンチエイリアシング設定
    // 今回はしない
    _graphicsPipelineDesc.SampleDesc.Count = 1;                     // サンプリングは1ピクセルにつき1
    _graphicsPipelineDesc.SampleDesc.Quality = 0;                   // クオリティ最低

    // ルートシグネチャー作成
    const auto FILTER = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // 線形補間
    MFramework::RootSignature rootSig;

    rootSig.Init(device.Get(), FILTER);

    _graphicsPipelineDesc.pRootSignature = rootSig.Get();

    // グラフィックスパイプラインステートオブジェクト作成
    ID3D12PipelineState* _pipelineState = nullptr;

    result = device->CreateGraphicsPipelineState(&_graphicsPipelineDesc, IID_PPV_ARGS(&_pipelineState));

    if (FAILED(result))
    {
        return -1;
    }

    // ビューポートを作成
    D3D12_VIEWPORT _viewport = {};

    _viewport.Width = static_cast<float>(test.GetWidth());
    _viewport.Height = static_cast<float>(test.GetHeight());
    _viewport.TopLeftX = 0;
    _viewport.TopLeftY = 0;
    _viewport.MaxDepth = 1.0f;
    _viewport.MinDepth = 0.0f;

    // シザー矩形
    // ビューポートに出力された画像のどこからどこまでを実際に画面に映し出すかを設定するためのもの
    D3D12_RECT _scissorRect = {};

    _scissorRect.top = 0;
    _scissorRect.left = 0;
    _scissorRect.right = _scissorRect.left + static_cast<LONG>(test.GetWidth());
    _scissorRect.bottom = _scissorRect.top + static_cast<LONG>(test.GetHeight());

    UINT frame = 0;

    MSG msg = {};

    float angle = 0.0f;

    #pragma region Main Loop
    // メインループ
    while(true)
    {
      angle += 0.1f;
      worldMatrix = DirectX::XMMatrixRotationY(angle);
      transformMatrix = worldMatrix * viewMatrix * projectionMatrix;

      constBuffer.Remap(1, &transformMatrix);

      if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }

      if (msg.message == WM_QUIT)
      {
        break;
      }

      // レンダーターゲットビューのインデックス取得
      UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

      // リソースバリアを設定
      D3D12_RESOURCE_BARRIER barrierDesc = {};

      barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;                      // 遷移
      barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;                           // 特に指定なし
      barrierDesc.Transition.pResource = _backBuffers[backBufferIndex];               // バックバッファーリソース
      barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;   // サブリソース番号
      barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;              // 直前はPRESENT状態
      barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;         // 今からレンダーターゲット状態

      // バリア指定実行
      // 第一引数:設定バリアの数(現時点では1でよい)
      // 第二引数:設定バリア構造体アドレス(配列で同時に設定できる)
      cmdList->ResourceBarrier(1, &barrierDesc);

      // パイプラインステートを設定
      cmdList->SetPipelineState(_pipelineState); 

      // 後の流れ:
      // レンダーターゲットを指定
      // 画面のクリアが終わったら
      // IDXGISwapchain::Present()メソッドを実行

      // レンダーターゲットを指定
      auto rtvHeap = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
      rtvHeap.ptr += static_cast<ULONG_PTR>(backBufferIndex * RTV_INCREMENT_SIZE);

      // レンダーターゲットビューをセット 
      // 第一引数:レンダーターゲット数(今回は一つだけで1でよい)
      // 第二引数:レンダーターゲットハンドル先頭アドレス
      // TODO 第三引数:複数時に連続しているか
      // 第四引数:深度ステンシルバッファービューのハンドル(nullptrでよい)
      cmdList->OMSetRenderTargets(1, &rtvHeap, false, nullptr);

      // 画面クリア
      float r = 0.0f;
      float g = 0.0f;
      float b = 0.0f;
      r = static_cast<float>((0xff & frame >> 16) / 255.0f);
      g = static_cast<float>((0xff & frame >> 8) / 255.0f);
      b = static_cast<float>((0xff & frame >> 0) / 255.0f);
      float clearColor[] = {r, g, b, 1.f};
      cmdList->ClearRenderTargetView(rtvHeap, clearColor, 0, nullptr);

      ++frame;

      // ルートシグネチャー設定
      cmdList->SetGraphicsRootSignature(rootSig.Get());
      cmdList->SetDescriptorHeaps(1, &basicDescHeap);
      cmdList->RSSetViewports(1, &_viewport);
      cmdList->RSSetScissorRects(1, &_scissorRect);

      cmdList->SetGraphicsRootDescriptorTable(
                                                0,           // ルートパラメーターインデックス 
                                                basicDescHeap->GetGPUDescriptorHandleForHeapStart()   // ヒープアドレス
                                              );

      cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      // 頂点バッファーを設定
      // 第一引数:スロット番号
      // 第二引数:頂点バッファービューの数
      // 第三引数:頂点バッファービューの配列

      D3D12_VERTEX_BUFFER_VIEW vertView = vertBuffer.GetView();
      D3D12_INDEX_BUFFER_VIEW idxView = idxBuffer.GetView();
      cmdList->IASetVertexBuffers(0, 1, &vertView);
      cmdList->IASetIndexBuffer(&idxView);
      
      // 描画命令を設定
      // 第一引数:頂点インデックス数
      // 第二引数:インスタンス数
      // 第三引数:頂点データのオフセット
      // 第四引数:インスタンスのオフセット
      // ※インスタンス数は同じプリミティブをいくつ表示するかという意味です
      cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);

      barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;    // 直前はレンダーターゲット状態
      barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;           // 今からPRESENT状態
      cmdList->ResourceBarrier(1, &barrierDesc);

      // ためておいた命令を実行
      // その前に命令をクローズが必須
      cmdList->Close();

      // 命令の実行
      ID3D12CommandList* cmdLists[] = { cmdList.Get()};

      // 第一引数:実行するコマンドリストの数(1でよい)
      // 第二引数:コマンドリスト配列の先頭アドレス
      cmdQueue.Execute(1, cmdLists);

      // フェンスを使ってGPUの処理が終わるまで待つ
      fence.Wait(cmdQueue.Get());

      // リセットし、命令オブジェクトをためていく
      // コマンドリストのクローズ状態を解除
      cmdList.Reset(static_cast<int>(backBufferIndex), _pipelineState);

      // フリップ
      // 第一引数:フリップまでの待ちフレーム数
      // ※0にするとPresentメソッドが即時復帰して次のフレームが始まってしまいます。
      // ※1にすると垂直同期を待つ
      // 第二引数:さまざまな指定を行います
      // テスト用出力やステレオをモノラル表示など特殊な用途であるため、今回は0にする
      swapChain->Present(1, 0);

      #pragma region Direct3D12  
      // DirectX処理
    } 
    #pragma endregion
    // end region of Main Loop

    // Dispose resources
    {
      test.Dispose();
      device.Dispose();
      dxgiFactory.Dispose();
      cmdList.Dispose();
      cmdQueue.Dispose();
      swapChain.Dispose();
      rootSig.Dispose();
      vertBuffer.Dispose();
      idxBuffer.Dispose();
      constBuffer.Dispose();
      vertShader.Dispose();
      pixelShader.Dispose();
    }

    return 0;
}
