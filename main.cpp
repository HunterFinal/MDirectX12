/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderFramework used by Game (Graphics API: DirectX12)

Update History: 2024/09/19 Create

Version : alpha_1.0.0

*/

// Debug include
#ifdef _DEBUG

#include <DebugHelper>

#endif
// end Debug include

// MWindow include
#include <Window/BaseWindow.h>
// end MWindow include

// DX12 include
#include <d3d12.h>
#include <dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#include <DirectXMath.h>

// end DX12 include

// shader compiler include
#include <d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

// end shader compiler include

#include <vector>
#include <string>

#pragma region DX12 Wrapper

#include <Graphics_DX12/DX12Device.h>
#include <Graphics_DX12/Fence.h>

#pragma endregion
// end region of DX12 Wrapper

#include <ComPtr.h>
#include <Graphics_DX12/Texture.h>
#include <RandomGenerator.hpp>

struct Vertex
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 uv;
};

#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
#endif
{
    // メモリリークを知らせる
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // Window
    MWindow::Window test;

    if(!test.InitWnd(1920, 1080, L"GameWindow", L"Framework_Window_Title"))
    {
        return -1;
    }

    #pragma region D3D12 DebugLayer

    #ifdef _DEBUG
        ID3D12Debug* _debugLayer = nullptr;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&_debugLayer))))
        {
            _debugLayer->EnableDebugLayer();
            _debugLayer->Release();
        }
    #endif

    #pragma endregion
    // end region of D3D12 DebugLayer

    #pragma region D3D12 Adapter and Device
    ComPtr<IDXGIFactory6> _dxgiFactory = nullptr;

    // アダプターを列挙するためDXGIFactoryオブジェクトを作成 
    HRESULT result = S_OK;

#ifdef _DEBUG                           
    result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(_dxgiFactory.GetAddressOf()));
#else
    result = CreateDXGIFactory1(IID_PPV_ARGS(_dxgiFactory.GetAddressOf()));
#endif

    if (FAILED(result))
    {
        return -1;
    }

    MFramework::DX12Device device;
    device.Init(_dxgiFactory.Get());

    #pragma endregion
    // end region of D3D12 Adapter and Device

    #pragma region D3D12 CommandAllocator and CommandList
    // コマンドリストの作成とコマンドアロケーター

    const auto CMD_LIST_TYPE = D3D12_COMMAND_LIST_TYPE_DIRECT;
    // 実際に命令をためているインターフェース
    ID3D12CommandAllocator* _cmdAllocator = nullptr;

    // 命令のインターフェース
    ID3D12GraphicsCommandList* _cmdList = nullptr;

    result = device->CreateCommandAllocator(CMD_LIST_TYPE, IID_PPV_ARGS(&_cmdAllocator));

    // 生成失敗
    if(result != S_OK)
    {
        return -1;
    }

    result = device->CreateCommandList(0, CMD_LIST_TYPE, _cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));

    // 生成失敗
    if(result != S_OK)
    {
        return -1;
    }
    #pragma endregion
    // end region of D3D12 CommandAllocator and CommandList

    #pragma region D3D12 CommandQueue
    // コマンドキュー
    // コマンドリストを用いてためた命令セットをGPUが順次実行していくためのもの

    ID3D12CommandQueue* _cmdQueue = nullptr;

    D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};

    // タイムアウトなし
    cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    // アタブターを一つしか使わないときは0でよい
    cmdQueueDesc.NodeMask = 0;

    // 優先順位は特に指定なし
    cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

    // コマンドリストと合わせる
    cmdQueueDesc.Type = CMD_LIST_TYPE;

    // キュー生成
    result = device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));

    // 生成失敗
    if(result != S_OK)
    {
        return -1;
    }
    #pragma endregion
    // end region of D3D12 CommandQueue

    #pragma region D3D12 SwapChain for Double buffering
    // スワップチェーンオブジェクト
    IDXGISwapChain4* _swapChain = nullptr;

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

    swapChainDesc.Width = 1920;
    swapChainDesc.Height = 1080;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Stereo = false;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
    swapChainDesc.BufferCount = 2;

    // バックバッファーは伸び縮み可能
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;

    // フリップ後は速やかに破棄
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    // 特に指定なし
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

    // ウインドウ⇔フルスクリーン切り替え可能
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // TODO 大事
    // 本来はQueryInterfaceなどを用いて IDXGISwapChain4*への変換チェックをするが、わかりやすさ重視のためキャストで対応
    result = _dxgiFactory->CreateSwapChainForHwnd(_cmdQueue, test.GetHWND(), &swapChainDesc, nullptr, nullptr, (IDXGISwapChain1**) &_swapChain);

    // 生成失敗
    if(result != S_OK)
    {
        return -1;
    }

    #pragma endregion
    // end region of D3D12 SwapChain for Double buffering

    #pragma region D3D12 Descriptor Heap for multiple Descriptors
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};

    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;     // レンダーターゲットビューなのでRTV
    heapDesc.NodeMask = 0;                              // GPUを一つだけ使用する想定なので0を入れる(複数のGPUがある場合に識別を行うためのビットフラグ)
    heapDesc.NumDescriptors = 2;                        // 裏表2つ
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

    result = _swapChain->GetDesc(&swcDesc);

    // 取得失敗
    if(result != S_OK)
    {
        return -1;
    }
    
    std::vector<ID3D12Resource*> _backBuffers(swcDesc.BufferCount);

    // Get DescriptorHeap handle
    D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();

    for(size_t i = 0; i < swcDesc.BufferCount; ++i)
    {
        result = _swapChain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(&_backBuffers[i]));
        device->CreateRenderTargetView(_backBuffers[i], nullptr, handle);

        // ハンドルサイズを計算し、先頭からずらす
        handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }
    #pragma endregion
    // end region of D3D12 SwapChain and Descriptor binding

    #pragma region D3D12 Fence
    // GPU側の処理が完了したかどうかを知るための仕組み

    MFramework::MGraphics_DX12::Fence fence;

    fence.Init(device);

    #pragma endregion
    // end region of D3D12 Fence
    // D3D12に使う定数
    // RTVのインクリメントサイズ
    const UINT RTV_INCREMENT_SIZE = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // 頂点座標
    // ※時計回り
    Vertex vertices[] = 
    {
        {{-0.4f, -0.7f, 0.f},{0.0f, 1.0f}},     // 左下
        {{-0.4f,  0.7f, 0.f},{0.0f, 0.0f}},     // 左上
        {{ 0.4f, -0.7f, 0.f},{1.0f, 1.0f}},     // 右下
        {{ 0.4f,  0.7f, 0.f},{1.0f, 0.0f}},     // 右上
    };
    
    // テクスチャ
    std::vector<MFramework::Color> textureData;
    textureData.resize(256*256);

    MUtility::MRandom::RandomGenerator<int> randomColorGenerator(0,255);

    for (auto& color : textureData)
    {
        color.R = randomColorGenerator.GenerateRandom();
        color.G = randomColorGenerator.GenerateRandom();
        color.B = randomColorGenerator.GenerateRandom();
        color.A = 255;
    }

    // テクスチャバッファーをCPUからGPUに転送する方法：
    // 1: ID3D12Resource::WriteToSubresource()メソッドを使う:比較的わかりやすいが特定の条件で効率が低下
    // 2: ID3D12Resource::Map()メソッドと、ID3D12GraphicsCommandList::CopyTextureRegion()/CopyBufferRegionメソッドを利用する:推奨されている

    // WriteToSubresourceで転送するためのヒープ設定
    D3D12_HEAP_PROPERTIES textureHeapProperty = {};

    // 特殊な設定なのでDEFAULTでもUPLOADでもない
    textureHeapProperty.Type = D3D12_HEAP_TYPE_CUSTOM;

    // ライトバック
    textureHeapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;

    // 転送はL0、つまりCPU側から直接行う
    textureHeapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

    // 単一アダプターのため0
    textureHeapProperty.CreationNodeMask = 0;
    textureHeapProperty.VisibleNodeMask = 0;

    D3D12_RESOURCE_DESC textureResourceDesc = {};

    textureResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureResourceDesc.Width = 256;
    textureResourceDesc.Height = 256;
    textureResourceDesc.DepthOrArraySize = 1;                           // 2Dで配列でもないので1
    textureResourceDesc.SampleDesc.Count = 1;                           // 通常テクスチャなのでアンチエイリアシングしない
    textureResourceDesc.SampleDesc.Quality = 0;                         // クオリティは最低
    textureResourceDesc.MipLevels = 1;                                  // ミップマップしないのでミップ数は一つ
    textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;          // レイアウトは決定しない
    textureResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;               // 特にフラグなし

    ID3D12Resource* texBuffer = nullptr;
    result = device->CreateCommittedResource(
                                                &textureHeapProperty,
                                                D3D12_HEAP_FLAG_NONE,
                                                &textureResourceDesc,
                                                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // テクスチャ用指定
                                                nullptr,
                                                IID_PPV_ARGS(&texBuffer)
                                            ); 

    if (FAILED(result))
    {
        return -1;
    }

    // テクスチャバッファーの転送
    result = texBuffer->WriteToSubresource(
                                            0,                                                                  // サブリソースインデックス
                                            nullptr,                                                            // 書き込み領域の指定(nullptrならば先頭から全領域となる)
                                            textureData.data(),                                                 // 書き込みたいデータのアドレス(元データアドレス)
                                            sizeof(MFramework::Color) * 256,                                    // 1ラインサイズ
                                            sizeof(MFramework::Color) * static_cast<UINT>(textureData.size())   // 全サイズ
                                          );

    if (FAILED(result))
    {
        return -1;
    }

    // テクスチャディスクリプタヒープ作成
    ID3D12DescriptorHeap* texDescHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC texDescHeapDesc = {};

    // シェーダーから見えるように
    texDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    // マスクは0
    texDescHeapDesc.NodeMask = 0;

    // ビューは今のところ1つだけ
    texDescHeapDesc.NumDescriptors = 1;

    // シェーダーリソースビュー用
    texDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

    // 生成
    result = device->CreateDescriptorHeap(&texDescHeapDesc, IID_PPV_ARGS(&texDescHeap));

    if (FAILED(result))
    {
        return -1;
    }

    // シェーダーリソースビューを作成
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;                                // RGBA(0.0f~1.0fに正規化)
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;                      // 2Dテクスチャ
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; // データのRGBAをどのようにマッピングするかということを指定するためのもの
                                                                                // 画像の情報がそのままRGBAなど「指定されたフォーマットに、
                                                                                // データどおりの順序で割り当てられている」ことを表しています。
    srvDesc.Texture2D.MipLevels = 1;                                            // ミップマップは使用しないので１

    // 三つ目の引数はディスクリプタヒープのどこにこのビューを配置するかを指定するためのもの
    // 複数のテクスチャビューがある場合、取得したハンドルからオフセットを指定する必要がある
    device->CreateShaderResourceView(
                                        texBuffer,                                          // ビューと関連付けるバッファー
                                        &srvDesc,                                           // 先ほど設定したテクスチャ設定情報
                                        texDescHeap->GetCPUDescriptorHandleForHeapStart()   // ヒープのどこに割り当てるか
                                    );

    D3D12_DESCRIPTOR_RANGE descTableRange = {};

    descTableRange.NumDescriptors = 1;                                                          // テクスチャ１つ（ディスクリプタ数）
    descTableRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;                                 // 種別はテクスチャ
    descTableRange.BaseShaderRegister = 0;                                                      // 0番スロットから
    descTableRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;    // 連続したディスクリプタレンジが前のディスクリプタレンジの直後に来る
    
    // ルートパラメーター作成
    D3D12_ROOT_PARAMETER rootParam = {};

    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

    // ピクセルシェーダーから見える
    rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // ディスクリプタレンジのアドレス
    rootParam.DescriptorTable.pDescriptorRanges = &descTableRange;

    // ディスクリプタレンジ数
    rootParam.DescriptorTable.NumDescriptorRanges = 1;

    // サンプラーを作成
    D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                     // 横方向の繰り返し
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                     // 縦方向の繰り返し
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                     // 奥行き方向の繰り返し
    samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;      // ボーダーは黒
    samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;                       // 線形補間
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;                                     // ミップマップ最大値
    samplerDesc.MinLOD = 0.0f;                                                  // ミップマップ最小値
    samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;               // ピクセルシェーダーから見える
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;                   // リサンプリングしない

    // 頂点バッファー作成
    // ID3D12Device::CreateCommitedResource()を使う
    D3D12_HEAP_PROPERTIES heapProperties = {};

    heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;                       // CPUからアクセスできる（Mapできる）※DEFAULTより遅い
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;   // TypeはCUSTOMじゃないためUNKNOWNでよい
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;    // TypeはCUSTOMじゃないためUNKNOWNでよい

    // 頂点バッファー情報
    D3D12_RESOURCE_DESC resourceDesc = {};

    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;       // バッファーに使うためBUFFERを指定
    resourceDesc.Width = sizeof(vertices);                          // 頂点情報が入るだけのサイズ
    resourceDesc.Height = 1;                                        // 幅で表現しているので１とする
    resourceDesc.DepthOrArraySize = 1;                              // 1でよい
    resourceDesc.MipLevels = 1;                                     // 1でよい
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;                      // 今回は画像ではないためUNKNOWNでよい
    resourceDesc.SampleDesc.Count = 1;                              // アンチエイリアシングを行うときのパラメーター。※ 0だとデータがないことになってしまうため、1にする
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;           // UNKNOWNと指定すると、自動で最適なレイアウトに設定しようとする　※今回はテクスチャではないため不適切です
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;                  // NONEでよい※要調べ

    ID3D12Resource* _vertBuffer = nullptr;

    if(FAILED(device->CreateCommittedResource( &heapProperties,
                                                D3D12_HEAP_FLAG_NONE,
                                                &resourceDesc,
                                                D3D12_RESOURCE_STATE_GENERIC_READ,
                                                nullptr,
                                                IID_PPV_ARGS(&_vertBuffer))))
    {
        return -1;
    }

    // 頂点情報のコピー(マップ)
    // コピーしないとGPUに渡すことができない
    Vertex* _vertMap = nullptr;

    // ID3D12Resource::Mapメソッド
    // 第一引数: ミップマップなどではないため0でよい　  ※リソース配列やミップマップの場合、サブリソース番号を渡す
    // 第二引数: 範囲指定。全範囲なのでnullptrでよい   ※一部のみを更新したいときは設定する
    // 第三引数: 受け取るためのポインター変数のアドレス
    if(FAILED(_vertBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_vertMap))))
    {
        return -1;
    }

    std::copy(std::begin(vertices), std::end(vertices), _vertMap);

    _vertBuffer->Unmap(0, nullptr);

    // 頂点バッファービューを作成
    D3D12_VERTEX_BUFFER_VIEW _vertexBufferView = {};

    _vertexBufferView.BufferLocation = _vertBuffer->GetGPUVirtualAddress(); // バッファーの仮想アドレス
    _vertexBufferView.SizeInBytes = sizeof(vertices);                       // 頂点の全バイト数
    _vertexBufferView.StrideInBytes = sizeof(vertices[0]);                  // 一頂点あたりのバイト数

    // 頂点インデックス作成
    UINT16 indices[] = {
                            0, 1, 2, 
                            2, 1, 3 
                       };

    ID3D12Resource* _indexBuffer = nullptr;
    // 設定は、バッファーのサイズ以外、頂点バッファーの設定を使い回してよい
    resourceDesc.Width = sizeof(indices);

    result = device->CreateCommittedResource(
                                                &heapProperties,
                                                D3D12_HEAP_FLAG_NONE,
                                                &resourceDesc,
                                                D3D12_RESOURCE_STATE_GENERIC_READ,
                                                nullptr,
                                                IID_PPV_ARGS(&_indexBuffer)
                                             );
    if(FAILED(result))
    {
        return -1;
    }

    UINT16* _mappedIndex = nullptr;
    _indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_mappedIndex));

    std::copy(std::begin(indices),std::end(indices),_mappedIndex);
    _indexBuffer->Unmap(0, nullptr);

    // インデックスバッファービュー作成
    D3D12_INDEX_BUFFER_VIEW _indexBufferView = {};

    _indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
    _indexBufferView.Format = DXGI_FORMAT_R16_UINT;
    _indexBufferView.SizeInBytes = sizeof(indices);

    // 頂点バッファービューを設定
    // 第一引数:※頂点バッファーの設定を開始するには、デバイスの 0 から始まる配列にインデックスを作成します。
    // 第二引数:渡す頂点バッファービューの数
    // 第三引数:頂点バッファービューの配列の先頭アドレス
    _cmdList->IASetVertexBuffers(0, 1, &_vertexBufferView);

    // Shaderオブジェクトを作成
    // ID3DBlobは　Binary Large Objectの略称、「何かのデータの塊」として使われている「汎用型」のオブジェクトです
    ID3DBlob* _vertexShaderBlob = nullptr;
    ID3DBlob* _pixelShaderBlob = nullptr;

    // エラーメッセージが入っているBlob
    ID3DBlob* _errorBlob = nullptr;

    const std::wstring VERTEX_SHADER = L"Shaders/HLSLs/BasicVertexShader.hlsl";
    const std::wstring PIXEL_SHADER = L"Shaders/HLSLs/BasicPixelShader.hlsl";
    const UINT SHADER_COMPILE_OPTION = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;  // デバッグ用および最適化なし

    // 頂点シェーダー作成

    result = D3DCompileFromFile(
                                    VERTEX_SHADER.c_str(),              // シェーダーファイル名
                                    nullptr,                            // #defineを配列で指定するための引数
                                    D3D_COMPILE_STANDARD_FILE_INCLUDE,  // shaderの中#includeが書かれているとき、インクルードファイルのディレクトリとしてカレントディレクトリを参照（#includeを使わないときはnullptrでよい）
                                    "BasicVS",                          // エントリーポイント
                                    "vs_5_0",                           // シェーダーモデルバージョン
                                    SHADER_COMPILE_OPTION,              // シェーダーコンパイルオプション
                                    0,                                  // エフェクトコンパイルオプション（シェーダーファイルの場合０が推奨）
                                    &_vertexShaderBlob,                 // 受け取るためのポインターのアドレス
                                    &_errorBlob                         // エラー用ポインターのアドレス
    );

    if (FAILED(result))
    {
        if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        {
            ::OutputDebugStringW(L"*****ERROR***** ファイルが見当たりません。\n");
        }
        else
        {
            std::string errMsg;
            errMsg.resize(_errorBlob->GetBufferSize());
            std::copy_n(static_cast<char*>(_errorBlob->GetBufferPointer()), _errorBlob->GetBufferSize(), errMsg.begin());

            ::OutputDebugStringA(errMsg.c_str());
        }

        return -1;
    }

    // ピクセルシェーダー作成
    result = D3DCompileFromFile(
                                    PIXEL_SHADER.c_str(),               // シェーダーファイル名
                                    nullptr,                            // #defineを配列で指定するための引数
                                    D3D_COMPILE_STANDARD_FILE_INCLUDE,  // shaderの中#includeが書かれているとき、インクルードファイルのディレクトリとしてカレントディレクトリを参照（#includeを使わないときはnullptrでよい）
                                    "BasicPS",                          // エントリーポイント
                                    "ps_5_0",                           // シェーダーモデルバージョン
                                    SHADER_COMPILE_OPTION,              // シェーダーコンパイルオプション
                                    0,                                  // エフェクトコンパイルオプション（シェーダーファイルの場合０が推奨）
                                    &_pixelShaderBlob,                  // 受け取るためのポインターのアドレス
                                    &_errorBlob                         // エラー用ポインターのアドレス
                               );

    if (FAILED(result))
    {
        if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        {
            ::OutputDebugStringW(L"*****ERROR***** ファイルが見当たりません。\n");
        }
        else
        {
            std::string errMsg;
            errMsg.resize(_errorBlob->GetBufferSize());
            std::copy_n(static_cast<char*>(_errorBlob->GetBufferPointer()), _errorBlob->GetBufferSize(), errMsg.begin());

            ::OutputDebugStringA(errMsg.c_str());
        }

        return -1;
    }

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
    _graphicsPipelineDesc.VS.pShaderBytecode = _vertexShaderBlob->GetBufferPointer();
    _graphicsPipelineDesc.VS.BytecodeLength = _vertexShaderBlob->GetBufferSize();
    _graphicsPipelineDesc.PS.pShaderBytecode = _pixelShaderBlob->GetBufferPointer();
    _graphicsPipelineDesc.PS.BytecodeLength = _pixelShaderBlob->GetBufferSize();

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
    _graphicsPipelineDesc.NumRenderTargets = 1;                         // 今回レンダーターゲットは一つしかないので1
    _graphicsPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;   // 0~1に正規化されたRGBA

    // アンチエイリアシング設定
    // 今回はしない
    _graphicsPipelineDesc.SampleDesc.Count = 1;                     // サンプリングは1ピクセルにつき1
    _graphicsPipelineDesc.SampleDesc.Quality = 0;                   // クオリティ最低

    // ルートシグネチャー作成
    // ディスクリプタテーブルをまとめたもの
    // 頂点情報以外のデータをパイプラインの外からシェーダーに送りこむために使われる
    // 今回は頂点だけを流し込んで、それをそのまま表示するため、ルートシグネチャー情報は特に必要ないが
    // 空のルートシグネチャーを設定しないとパイプラインステートは作れません
    ID3D12RootSignature* _rootSignature = nullptr;
    D3D12_ROOT_SIGNATURE_DESC _rootSignatureDesc = {};

    // Flagsに「頂点情報（入力アセンブラ）がある」という意味の値を設定
    _rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    _rootSignatureDesc.pParameters = &rootParam;    // ルートパラメーターの先頭アドレス
    _rootSignatureDesc.NumParameters = 1;           // ルートパラメーター数
    _rootSignatureDesc.pStaticSamplers = &samplerDesc;
    _rootSignatureDesc.NumStaticSamplers = 1;

    // バイナリコード作成
    ID3DBlob* _rootSigBlob = nullptr;
    result = D3D12SerializeRootSignature(
                                            &_rootSignatureDesc,            // ルートシグネチャー設定
                                            D3D_ROOT_SIGNATURE_VERSION_1_0, // ルートシグネチャーバージョン     ※1.1については要調査//ID3D12Device::CheckFeatureSupport()
                                            &_rootSigBlob,                  // シェーダーを作ったときと同じ
                                            &_errorBlob                     // エラーメッセージ
                                        ); 

    if(FAILED(result))
    {
        std::string errMsg;
        errMsg.resize(_errorBlob->GetBufferSize());
        std::copy_n(static_cast<char*>(_errorBlob->GetBufferPointer()), _errorBlob->GetBufferSize(), errMsg.begin());

        ::OutputDebugStringA(errMsg.c_str());
    }

    // ルートシグネチャーオブジェクト作成
    result = device->CreateRootSignature(
                                            0,      // nodemask 0でよい
                                            _rootSigBlob->GetBufferPointer(),
                                            _rootSigBlob->GetBufferSize(),
                                            IID_PPV_ARGS(&_rootSignature)
                                         );
    
    if(FAILED(result))
    {
        std::string errMsg;
        errMsg.resize(_errorBlob->GetBufferSize());
        std::copy_n(static_cast<char*>(_errorBlob->GetBufferPointer()), _errorBlob->GetBufferSize(), errMsg.begin());

        ::OutputDebugStringA(errMsg.c_str());
    }

    // 不要になったので解放
    _rootSigBlob->Release();

    _graphicsPipelineDesc.pRootSignature = _rootSignature;

    // グラフィックスパイプラインステートオブジェクト作成
    ID3D12PipelineState* _pipelineState = nullptr;

    result = device->CreateGraphicsPipelineState(&_graphicsPipelineDesc, IID_PPV_ARGS(&_pipelineState));

    if (FAILED(result))
    {
        return -1;
    }

    // ビューポートを作成
    D3D12_VIEWPORT _viewport = {};

    _viewport.Width = 1920;
    _viewport.Height = 1080;
    _viewport.TopLeftX = 0;
    _viewport.TopLeftY = 0;
    _viewport.MaxDepth = 1.0f;
    _viewport.MinDepth = 0.0f;

    // シザー矩形
    // ビューポートに出力された画像のどこからどこまでを実際に画面に映し出すかを設定するためのもの
    D3D12_RECT _scissorRect = {};

    _scissorRect.top = 0;
    _scissorRect.left = 0;
    _scissorRect.right = _scissorRect.left + 1920;
    _scissorRect.bottom = _scissorRect.top + 1080;

    UINT frame = 0;

    MSG msg = {};

    #pragma region Main Loop
    // メインループ
    while(true)
    {

        if (PeekMessage(&msg,nullptr,0,0,PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
        {
            break;
        }

        // レンダーターゲットビューのインデックス取得
        UINT backBufferIndex = _swapChain->GetCurrentBackBufferIndex();

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
        _cmdList->ResourceBarrier(1, &barrierDesc);

        // パイプラインステートを設定
        _cmdList->SetPipelineState(_pipelineState); 

        
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
        _cmdList->OMSetRenderTargets(1, &rtvHeap, false, nullptr);

        // 画面クリア
        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;
        r = static_cast<float>((0xff & frame >> 16) / 255.0f);
        g = static_cast<float>((0xff & frame >> 8) / 255.0f);
        b = static_cast<float>((0xff & frame >> 0) / 255.0f);
        float clearColor[] = {r, g, b, 1.f};
        _cmdList->ClearRenderTargetView(rtvHeap, clearColor, 0, nullptr);

        ++frame;

        // ルートシグネチャー設定
        _cmdList->RSSetViewports(1, &_viewport);
        _cmdList->RSSetScissorRects(1, &_scissorRect);
        _cmdList->SetGraphicsRootSignature(_rootSignature);
        _cmdList->SetDescriptorHeaps(1, &texDescHeap);
        _cmdList->SetGraphicsRootDescriptorTable(
                                                    0,                                                 // ルートパラメーターインデックス 
                                                    texDescHeap->GetGPUDescriptorHandleForHeapStart()  // ヒープアドレス
                                                );

        _cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // 頂点バッファーを設定
        // 第一引数:スロット番号
        // 第二引数:頂点バッファービューの数
        // 第三引数:頂点バッファービューの配列
        _cmdList->IASetVertexBuffers(0, 1, &_vertexBufferView);
        _cmdList->IASetIndexBuffer(&_indexBufferView);
        
        // 描画命令を設定
        // 第一引数:頂点数
        // 第二引数:インスタンス数
        // 第三引数:頂点データのオフセット
        // 第四引数:インスタンスのオフセット
        // ※インスタンス数は同じプリミティブをいくつ表示するかという意味です
        _cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);

        barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;    // 直前はレンダーターゲット状態
        barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;           // 今からPRESENT状態
        _cmdList->ResourceBarrier(1, &barrierDesc);

        // ためておいた命令を実行
        // その前に命令をクローズが必須
        _cmdList->Close();

        // 命令の実行
        ID3D12CommandList* cmdLists[] = { _cmdList };

        // 第一引数:実行するコマンドリストの数(1でよい)
        // 第二引数:コマンドリスト配列の先頭アドレス
        _cmdQueue->ExecuteCommandLists(1, cmdLists);

        // フェンスを使ってGPUの処理が終わるまで待つ
        fence.Wait(_cmdQueue);

        // リセットし、命令オブジェクトをためていく
        // コマンドリストのクローズ状態を解除
        _cmdAllocator->Reset();
        _cmdList->Reset(_cmdAllocator, _pipelineState);

        // フリップ
        // 第一引数:フリップまでの待ちフレーム数
        // ※0にするとPresentメソッドが即時復帰して次のフレームが始まってしまいます。
        // ※1にすると垂直同期を待つ
        // 第二引数:さまざまな指定を行います
        // テスト用出力やステレオをモノラル表示など特殊な用途であるため、今回は0にする
        _swapChain->Present(1, 0);

        #pragma region Direct3D12  
        // DirectX処理
    } 
    #pragma endregion
    // end region of Main Loop

    test.Dispose();

    return 0;
}