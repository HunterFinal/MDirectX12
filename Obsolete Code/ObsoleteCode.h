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
#pragma endregion
// D3D12
    