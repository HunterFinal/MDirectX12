/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderFramework used by Game (Graphics API: DirectX12)

Update History: 2024/09/19 Create

Version : alpha_1.0.0

*/

// Debug include
#include <DebugHelper>
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

#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
#endif
{

    // Window
    MWindow::Window test;

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

    // アタブターを探す
    for (int i = 0; _dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        adapters.push_back(tmpAdapter);
    }

    tmpAdapter = nullptr;

    // アダプターを識別するための情報を取得
    for(auto adpt : adapters)
    {
        DXGI_ADAPTER_DESC adesc = {};
        // アダプターの説明オブジェクト取得
        adpt->GetDesc(&adesc);

        std::wstring strDesc = adesc.Description;

        // 探したいアダプターの名前を確認
        if(strDesc.find(L"NVIDIA") != std::wstring::npos)
        {
            tmpAdapter = adpt;
            break;
        }
    }

    D3D_FEATURE_LEVEL levels[] = 
    {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D_FEATURE_LEVEL featureLevel;

    for(auto lv : levels)
    {
        if(D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&_device)) == S_OK)
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

    const auto cmdListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
    // 実際に命令をためているインターフェース
    ID3D12CommandAllocator* _cmdAllocator = nullptr;

    // 命令のインターフェース
    ID3D12GraphicsCommandList* _cmdList = nullptr;

    result = _device->CreateCommandAllocator(cmdListType, IID_PPV_ARGS(&_cmdAllocator));

    // 生成失敗
    if(result != S_OK)
    {
        return -1;
    }

    result = _device->CreateCommandList(0, cmdListType, _cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));

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
    cmdQueueDesc.Type = cmdListType;

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
    result = _dxgiFactory->CreateSwapChainForHwnd(_cmdQueue, test.GetHandle(), &swapChainDesc, nullptr, nullptr, (IDXGISwapChain1**) &_swapChain);

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
    ID3D12Fence* _fence = nullptr;
    UINT64 _fenceVal = 0;

    result = _device->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
    #pragma endregion
    // end region of D3D12 Fence
    // D3D12に使う定数
    // RTVのインクリメントサイズ
    const UINT RTV_INCREMENT_SIZE = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


    MSG msg = {};

    // 頂点座標
    // ※時計回り
    DirectX::XMFLOAT3 vertices[] = 
    {
        {-1.f, -1.f, 0.f},  // 左下
        {-1.f,  1.f, 0.f},  // 左上
        { 1.f, -1.f, 0.f},  // 右下
    };

    // 頂点バッファー作成
    // ID3D12Device::CreateCommitedResource()を使う
    D3D12_HEAP_PROPERTIES heapProperties = {};

    heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;                       // CPUからアクセスできる（Mapできる）※DEFAULTより遅い
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;   // TypeはCUSTOMじゃないためUNKNOWNでよい
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;    // TypeはCUSTOMじゃないためUNKNOWNでよい

    // 頂点バッファー
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

            OutputDebugStringA(errMsg.c_str());
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
            "Position",                                 // セマンティクス名(今回は座標なのでPOSITION)
            0,                                          // 同じセマンティクス名のときに使うインデックス(0でよい) ※要調べ
            DXGI_FORMAT_R32G32B32_FLOAT,                // フォーマット(データの解釈(float3のため,rbg毎に32bit,float型))
            0,                                          // 入力スロットインデックス(0でよい)IASetVertexBuffersでスロットとバッファーとを関連付けている
            D3D12_APPEND_ALIGNED_ELEMENT,               // データが連続していることを表す定義としてALIGNED_ELEMENT
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, // データの内容として、1頂点ごとにこのレイアウトが入っているを表す列挙※13.2.2で説明するらしい
            0                                           // 一度に描画するインスタンスの数(インスタンシングを使わないため0でよい)※13.2.2で説明するらしい
        },
    };

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

        barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;              // 遷移
        barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;                   // 特に指定なし
        barrierDesc.Transition.pResource = _backBuffers[backBufferIndex];       // バックバッファーリソース
        barrierDesc.Transition.Subresource = 0;                                 // サブリソース番号
        barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;      // 直前はPRESENT状態
        barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 今からレンダーターゲット状態

        // バリア指定実行
        // 第一引数:設定バリアの数(現時点では1でよい)
        // 第二引数:設定バリア構造体アドレス(配列で同時に設定できる)
        _cmdList->ResourceBarrier(1, &barrierDesc);
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
        _cmdList->OMSetRenderTargets(1, &rtvHeap, true, nullptr);

        // 画面クリア
        float clearColor[] = {1.f, 1.f, 0.f, 1.f};      // 黄色
        _cmdList->ClearRenderTargetView(rtvHeap, clearColor, 0, nullptr);

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
        _cmdQueue->Signal(_fence, ++_fenceVal);

        if(_fence->GetCompletedValue() != _fenceVal)
        {
            // イベントハンドルの取得
            HANDLE event = CreateEvent(nullptr, false, false, nullptr);

            if (event == nullptr)
            {
                return -1;
            }

            _fence->SetEventOnCompletion(_fenceVal, event);

            // イベントが発生するまで待ち続ける
            WaitForSingleObject(event, INFINITE);

            // イベントハンドルを閉じる
            CloseHandle(event);
        }

        // リセットし、命令オブジェクトをためていく
        // コマンドリストのクローズ状態を解除
        _cmdAllocator->Reset();
        _cmdList->Reset(_cmdAllocator, nullptr);

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

    return 0;
}