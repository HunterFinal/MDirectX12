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

#include <Graphics_DX12/Fence.h>

#pragma endregion
// end region of DX12 Wrapper

#include <RandomGenerator.hpp>

#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
#endif
{
    // メモリリークを知らせる
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    MUtility::MRandom::RandomGenerator<int> generator(0, 10);

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
    ID3D12Device* _device = nullptr;
    IDXGIFactory6* _dxgiFactory = nullptr;

    // アダプターを列挙するためDXGIFactoryオブジェクトを作成 
    HRESULT result = S_OK;

    if (FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&_dxgiFactory))))
    {
        if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory))))
        {
            return -1;
        }
    }
    
    // 利用可能なアタブターの列挙用
    std::vector<IDXGIAdapter*> adapters;

    IDXGIAdapter* tmpAdapter = nullptr;

    enum class GPULevel : INT8
    {
        NVIDIA      = 5,
        Amd         = 4,
        Intel       = 3,
        Arm         = 2,
        Qualcomm    = 1,

        Lowest      = -128,

    };

    int adapterIndex = 0;
    // アタブターを探す
    while(_dxgiFactory->EnumAdapters(adapterIndex, &tmpAdapter) != DXGI_ERROR_NOT_FOUND)
    {
        adapters.push_back(tmpAdapter);
        ++adapterIndex;
    }

    tmpAdapter = nullptr;

    GPULevel level = GPULevel::Lowest;

    // アダプターを識別するための情報を取得
    for(const auto& adpt : adapters)
    {
        DXGI_ADAPTER_DESC adesc = {};
        // アダプターの説明オブジェクト取得
        adpt->GetDesc(&adesc);

        std::wstring strDesc(adesc.Description);

        // ::OutputDebugString(strDesc.c_str());
        // ::OutputDebugString(L"\n");

        // 探したいアダプターの名前を確認
        if (strDesc.find(L"NVIDIA") != std::wstring::npos)
        {
            if (level < GPULevel::NVIDIA)
            {
                level = GPULevel::NVIDIA;
                tmpAdapter = adpt;
            }
        }
        else if (strDesc.find(L"Amd") != std::wstring::npos)
        {
            if (level < GPULevel::Amd)
            {
                level = GPULevel::Amd;
                tmpAdapter = adpt;
            }
        }
        else if (strDesc.find(L"Intel") != std::wstring::npos)
        {
            if (level < GPULevel::Intel)
            {
                level = GPULevel::Intel;
                tmpAdapter = adpt;
            }
        }
        else if (strDesc.find(L"Arm") != std::wstring::npos)
        {
            if (level < GPULevel::Arm)
            {
                level = GPULevel::Arm;
                tmpAdapter = adpt;
            }
        }
        else if (strDesc.find(L"Qualcomm") != std::wstring::npos)
        {
            if (level < GPULevel::Qualcomm)
            {
                level = GPULevel::Qualcomm;
                tmpAdapter = adpt;
            }
        }
    }

    D3D_FEATURE_LEVEL levels[] = 
    {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D_FEATURE_LEVEL featureLevel;

    for(auto lv : levels)
    {
        if(D3D12CreateDevice(tmpAdapter, lv, IID_PPV_ARGS(&_device)) == S_OK)
        {
            featureLevel = lv;
            break;
        }
    }

    // 生成失敗
    if(_device == nullptr)
    {
        return -1;
    }
    #pragma endregion
    // end region of D3D12 Adapter and Device

    #pragma region D3D12 CommandAllocator and CommandList
    // コマンドリストの作成とコマンドアロケーター

    const auto CMD_LIST_TYPE = D3D12_COMMAND_LIST_TYPE_DIRECT;
    // 実際に命令をためているインターフェース
    ID3D12CommandAllocator* _cmdAllocator = nullptr;

    // 命令のインターフェース
    ID3D12GraphicsCommandList* _cmdList = nullptr;

    result = _device->CreateCommandAllocator(CMD_LIST_TYPE, IID_PPV_ARGS(&_cmdAllocator));

    // 生成失敗
    if(result != S_OK)
    {
        return -1;
    }

    result = _device->CreateCommandList(0, CMD_LIST_TYPE, _cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));

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
    result = _device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));

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

    result = _device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));

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
        _device->CreateRenderTargetView(_backBuffers[i], nullptr, handle);

        // ハンドルサイズを計算し、先頭からずらす
        handle.ptr += _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }
    #pragma endregion
    // end region of D3D12 SwapChain and Descriptor binding

    #pragma region D3D12 Fence
    // GPU側の処理が完了したかどうかを知るための仕組み

    MFramework::MGraphics_DX12::Fence fence;

    fence.Init(_device);

    #pragma endregion
    // end region of D3D12 Fence
    // D3D12に使う定数
    // RTVのインクリメントサイズ
    const UINT RTV_INCREMENT_SIZE = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // 頂点座標
    // ※時計回り
    DirectX::XMFLOAT3 vertices[] = 
    {
        {-0.4f, -0.7f, 0.f},  // 左下
        {-0.4f,  0.7f, 0.f},  // 左上
        { 0.4f, -0.7f, 0.f},  // 右下
        { 0.4f,  0.7f, 0.f},  // 右上
    };

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

    if(FAILED(_device->CreateCommittedResource( &heapProperties,
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
    DirectX::XMFLOAT3* _vertMap = nullptr;

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

    result = _device->CreateCommittedResource(
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
        {
            "POSITION",                                 // セマンティクス名(今回は座標なのでPOSITION)
            0,                                          // 同じセマンティクス名のときに使うインデックス(0でよい) ※要調べ
            DXGI_FORMAT_R32G32B32_FLOAT,                // フォーマット(データの解釈(float3のため,rbg毎に32bit,float型))
            0,                                          // 入力スロットインデックス(0でよい)IASetVertexBuffersでスロットとバッファーとを関連付けている
            D3D12_APPEND_ALIGNED_ELEMENT,               // データが連続していることを表す定義としてALIGNED_ELEMENT
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // データの内容として、1頂点ごとにこのレイアウトが入っているを表す列挙※13.2.2で説明するらしい
            0                                           // 一度に描画するインスタンスの数(インスタンシングを使わないため0でよい)※13.2.2で説明するらしい
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
    result = _device->CreateRootSignature(
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

    result = _device->CreateGraphicsPipelineState(&_graphicsPipelineDesc, IID_PPV_ARGS(&_pipelineState));

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