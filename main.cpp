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
            auto event = CreateEvent(nullptr, false, false, nullptr);

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