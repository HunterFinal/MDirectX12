/*

MRenderFramework
Author : MAI ZHICONG

Description : Texture of MRenderFramework (Graphics API: DirectX12)

Update History: 2024/11/19

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <Graphics_DX12/Texture.h>
#include <Graphics_DX12/CommandList.h>
#include <Graphics_DX12/Fence.h>

#include <d3d12.h>
#include <DirectXTex.h>

#include <FileUtil.h>
#include <D3D12EasyUtil.h>

#include <string>
#include <cassert>

namespace MFramework
{
  #if HAS_CPP_11
    using TexMetadata = DirectX::TexMetadata;
    using ScratchImg  = DirectX::ScratchImage;
    using Img         = DirectX::Image;
  #else
    typedef TexMetadata DirectX::TexMetadata;
    typedef ScratchImg  DirectX::ScratchImage;
    typedef Img         DirectX::Image;
  #endif

  Texture::Texture()
    : m_tex(nullptr)
  {}

  Texture::~Texture()
  {
    Dispose();
  }

  bool Texture::Create( ID3D12Device* device, 
                        CommandList* cmdList,
                        ID3D12CommandQueue* cmdQueue,
                        DescriptorHandle handle,
                        const wchar_t* fileName)
  {
    if (device == nullptr || cmdList == nullptr || cmdQueue == nullptr)
    {
      return false;
    }

    m_handle = handle;

    std::wstring filePath;
    if (!FileUtility::SearchFilePath(fileName, filePath))
    {
      return false;
    }

    HRESULT result = S_OK;
    TexMetadata metadata;
    ScratchImg scratchImg;

    result = DirectX::LoadFromWICFile(                                          
                                      filePath.c_str(),                 // ファイルパス
                                      DirectX::WIC_FLAGS_NONE,         // どのようにロードするかを示すフラグ
                                      &metadata,                       // メタデータ(DirectX::TexMetadata)を受け取るためのポインター
                                      scratchImg                       // 実際のデータが入っているオブジェクト
                                     ); 
    
    if (FAILED(result))
    {
      return false;
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
    resDesc.Width = D3D12EasyUtility::AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * img->height;    // データサイズ
    resDesc.Height = 1;
    resDesc.DepthOrArraySize = 1;
    resDesc.MipLevels = 1;
    resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;  // 連続したデータ
    resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;         // 特にフラグなし
    resDesc.SampleDesc.Count = 1; //通常テクスチャなのでアンチエイリアシングしない
    resDesc.SampleDesc.Quality = 0;

    ComPtr<ID3D12Resource> uploadBuffer;
    // 中間バッファー作成
    result = device->CreateCommittedResource(
                                              &uploadHeapProp,
                                              D3D12_HEAP_FLAG_NONE, //特にフラグなし
                                              &resDesc,
                                              D3D12_RESOURCE_STATE_GENERIC_READ,    // CPUから書き込み可能だが、GPUから見ると読み取り専用
                                              nullptr,
                                              IID_PPV_ARGS(uploadBuffer.ReleaseAndGetAddressOf())
                                            );     

    if (FAILED(result))
    {
      return false;
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

    result = device->CreateCommittedResource(
                                              &texHeapProp,
                                              D3D12_HEAP_FLAG_NONE,
                                              &resDesc,
                                              D3D12_RESOURCE_STATE_COPY_DEST, // コピー先
                                              nullptr,
                                              IID_PPV_ARGS(m_tex.ReleaseAndGetAddressOf())
                                            );
    if (FAILED(result))
    {
      return false;
    }
    
    // アップロードリソースへのマップ
    UINT8* mapforImg = nullptr; // img->pixelsと同じ型にする
    result = uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mapforImg));

    // 元データをコピーする際に、元データのRowPitchとバッファーのRowPitchが合わないため、
    // 一行ごとにコピーして行頭が合うようにする
    UINT8* srcAddress = img->pixels;
    size_t rowPitch = D3D12EasyUtility::AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

    for (int y = 0; y < img->height; ++y)
    {
      std::copy_n(srcAddress, rowPitch, mapforImg);

      // 一行ごとの辻褄を合わせる
      srcAddress += img->rowPitch;
      mapforImg += rowPitch;
    }

    uploadBuffer->Unmap(0, nullptr); //アンマップ

    {
      D3D12_TEXTURE_COPY_LOCATION src = {};
      // コピー元(アップロード側)設定
      src.pResource = uploadBuffer.Get();   // 中間バッファー
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
      src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(D3D12EasyUtility::AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));
      src.PlacedFootprint.Footprint.Format = img->format;

      D3D12_TEXTURE_COPY_LOCATION dst = {};

      // コピー先設定
      dst.pResource = m_tex.Get();
      dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
      dst.SubresourceIndex = 0;
      cmdList->Get()->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

      D3D12_RESOURCE_BARRIER barrierDesc = {};

      barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
      barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
      barrierDesc.Transition.pResource = m_tex.Get();
      barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
      barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;    // ここが重要
      barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // ここが重要

      cmdList->Get()->ResourceBarrier(1, &barrierDesc);
      cmdList->Get()->Close();

      // コマンドリストの実行
      ID3D12CommandList* cmdLists[] = { cmdList->Get(),};
      cmdQueue->ExecuteCommandLists(1, cmdLists);

      // TODO
      // 一時的なFenceを作成
      Fence fence;
      fence.Init(device);
      fence.Wait(cmdQueue);
      fence.Dispose();

      cmdList->Reset(0, nullptr);
    }

    {
      // シェーダーリソースビューを作成
      D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

      auto desc = m_tex->GetDesc();

      srvDesc.Format = metadata.format;                                               // RGBA(0.0f~1.0fに正規化)
      srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;                      // 2Dテクスチャ
      srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; // データのRGBAをどのようにマッピングするかということを指定するためのもの
                                                                                  // 画像の情報がそのままRGBAなど「指定されたフォーマットに、
                                                                                  // データどおりの順序で割り当てられている」ことを表しています。
      srvDesc.Texture2D.MipLevels = 1;                                            // ミップマップは使用しないので１

      // 三つ目の引数はディスクリプタヒープのどこにこのビューを配置するかを指定するためのもの
      // 複数のテクスチャビューがある場合、取得したハンドルからオフセットを指定する必要がある
      if (m_handle.HasCPUHandle())
      {
        device->CreateShaderResourceView(
                                          m_tex.Get(),                                          // ビューと関連付けるバッファー
                                          &srvDesc,                                           // 先ほど設定したテクスチャ設定情報
                                          m_handle.CPUHandle                           // ヒープのどこに割り当てるか
                                        );
      }
      else
      {
        return false;
      }
    }

    uploadBuffer.Reset();
    return true;
  }

  void Texture::Dispose() noexcept
  {
    m_tex.Reset();
  }
}