#error Don't import this file

#pragma region D3D12

#pragma region Fence

// ID3D12Fence* _fence = nullptr;
// UINT64 _fenceVal = 0;

// result = _device->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

// _cmdQueue->Signal(_fence, ++_fenceVal);

// if(_fence->GetCompletedValue() != _fenceVal)
// {
//     // イベントハンドルの取得
//     HANDLE event = CreateEvent(nullptr, false, false, nullptr);

//     if (event == nullptr)
//     {
//         return -1;
//     }

//     _fence->SetEventOnCompletion(_fenceVal, event);

//     // イベントが発生するまで待ち続ける
//     WaitForSingleObject(event, INFINITE);

//     // イベントハンドルを閉じる
//     CloseHandle(event);
// }
#pragma endregion
// Fence

#pragma region Texture
 // TODO test texture
    // std::vector<MFramework::Color> textureData;
    // textureData.resize(256*256);

    // MUtility::MRandom::RandomGenerator<int> randomColorGenerator(0,255);

    // for (auto& color : textureData)
    // {
    //     color.R = randomColorGenerator.GenerateRandom();
    //     color.G = randomColorGenerator.GenerateRandom();
    //     color.B = randomColorGenerator.GenerateRandom();
    //     color.A = 255;
    // }

    // テクスチャバッファーをCPUからGPUに転送する方法：
    // 1: ID3D12Resource::WriteToSubresource()メソッドを使う:比較的わかりやすいが特定の条件で効率が低下
    // 2: ID3D12Resource::Map()メソッドと、ID3D12GraphicsCommandList::CopyTextureRegion()/CopyBufferRegionメソッドを利用する:推奨されている

    // // WriteToSubresourceで転送するためのヒープ設定
    // D3D12_HEAP_PROPERTIES textureHeapProperty = {};

    // // 特殊な設定なのでDEFAULTでもUPLOADでもない
    // textureHeapProperty.Type = D3D12_HEAP_TYPE_CUSTOM;

    // // ライトバック
    // textureHeapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;

    // // 転送はL0、つまりCPU側から直接行う
    // textureHeapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

    // // 単一アダプターのため0
    // textureHeapProperty.CreationNodeMask = 0;
    // textureHeapProperty.VisibleNodeMask = 0;

    // D3D12_RESOURCE_DESC textureResourceDesc = {};

    // textureResourceDesc.Format = metadata.format;
    // textureResourceDesc.Width = metadata.width;
    // textureResourceDesc.Height = metadata.height;
    // textureResourceDesc.DepthOrArraySize = metadata.arraySize;                                  // 2Dで配列でもないので1
    // textureResourceDesc.SampleDesc.Count = 1;                                                   // 通常テクスチャなのでアンチエイリアシングしない
    // textureResourceDesc.SampleDesc.Quality = 0;                                                 // クオリティは最低
    // textureResourceDesc.MipLevels = metadata.mipLevels;                                         // ミップマップしないのでミップ数は一つ
    // textureResourceDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
    // textureResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;                                  // レイアウトは決定しない
    // textureResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;                                       // 特にフラグなし

    // ID3D12Resource* texBuffer = nullptr;
    // result = device->CreateCommittedResource(
    //                                             &textureHeapProperty,
    //                                             D3D12_HEAP_FLAG_NONE,
    //                                             &textureResourceDesc,
    //                                             D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // テクスチャ用指定
    //                                             nullptr,
    //                                             IID_PPV_ARGS(&texBuffer)
    //                                         ); 

    // テクスチャバッファーの転送
    // 効率低下が生じる場合がある
    // result = texBuffer->WriteToSubresource(
    //                                         0,                // サブリソースインデックス
    //                                         nullptr,          // 書き込み領域の指定(nullptrならば先頭から全領域となる)
    //                                         img->pixels,      // 書き込みたいデータのアドレス(元データアドレス)
    //                                         img->rowPitch,    // 1ラインサイズ
    //                                         img->slicePitch   // 1枚サイズ
    //                                       );

    // TODO Random Color Pixel
    // result = texBuffer->WriteToSubresource(
    //                                         0,                                                                  // サブリソースインデックス
    //                                         nullptr,                                                            // 書き込み領域の指定(nullptrならば先頭から全領域となる)
    //                                         textureData.data(),                                                 // 書き込みたいデータのアドレス(元データアドレス)
    //                                         sizeof(MFramework::Color) * 256,                                    // 1ラインサイズ
    //                                         sizeof(MFramework::Color) * static_cast<UINT>(textureData.size())   // 全サイズ
    //                                       );
#pragma endregion
// Texture

#pragma endregion
// D3D12
    