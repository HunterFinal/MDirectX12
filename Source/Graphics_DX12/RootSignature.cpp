/*

MRenderFramework
Author : MAI ZHICONG

Description : DirectX12 RootSignature Wrapper (Graphics API: DirectX12)

Update History: 2024/11/07 Create
           
Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <Graphics_DX12/RootSignature.h>

#include <d3d12.h>
#include <string>

#include <cassert>

namespace
{
  const D3D12_ROOT_SIGNATURE_FLAGS ROOT_SIGNATURE_FLAGS = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
                                                        | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
                                                        | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
                                                        | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
}

namespace MFramework
{
  RootSignature::RootSignature()
    :m_rootSignature(nullptr)
  { }
  RootSignature::~RootSignature()
  {
    Dispose();
  }
  void RootSignature::Init(ID3D12Device* device, D3D12_FILTER filter)
  {
    if (device == nullptr)
    {
      return;
    }
    // テクスチャ用
    D3D12_DESCRIPTOR_RANGE ranges[2] = {};

    ranges[0].NumDescriptors = 1;                                                          // テクスチャ１つ（ディスクリプタ数）
    ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;                                 // 種別はテクスチャ
    ranges[0].BaseShaderRegister = 0;                                                      // 0番スロットから
    ranges[0].RegisterSpace = 0;
    ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;    // 連続したディスクリプタレンジが前のディスクリプタレンジの直後に来る

    // 定数用レジスター 0番
    ranges[1].NumDescriptors = 1;                                                          // 定数１つ（ディスクリプタ数）
    ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;                                 // 種別は定数
    ranges[1].BaseShaderRegister = 0;                                                      // 0番スロットから
    ranges[1].RegisterSpace = 0;
    ranges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;    // 連続したディスクリプタレンジが前のディスクリプタレンジの直後に来る

      // ルートパラメーター作成
    D3D12_ROOT_PARAMETER rootParam = {};

    // テクスチャと定数用
    // シェーダーリソースビューと定数バッファービューが連続しており、またレンジも連続しているため、
    // ルートパラメーター1つに対して[レンジが2つ]という指定を行えば、いっぺんに2つのレジスター設定ができ、切り替えコストも軽減できる
    rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    // ディスクリプタレンジのアドレス
    rootParam.DescriptorTable.pDescriptorRanges = &ranges[0];
    // ディスクリプタレンジ数
    rootParam.DescriptorTable.NumDescriptorRanges = 2;
    // すべてのシェーダーから見える
    rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    //   // ルートパラメーター作成
    // D3D12_ROOT_PARAMETER rootParam[2] = {};

    // // テクスチャ用
    // // シェーダーリソースビューと定数バッファービューが連続しており、またレンジも連続しているため、
    // // ルートパラメーター1つに対して[レンジが2つ]という指定を行えば、いっぺんに2つのレジスター設定ができ、切り替えコストも軽減できる
    // rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    // // ディスクリプタレンジのアドレス
    // rootParam[0].DescriptorTable.pDescriptorRanges = &ranges[0];
    // // ディスクリプタレンジ数
    // rootParam[0].DescriptorTable.NumDescriptorRanges = 1;
    // // すべてのシェーダーから見える
    // rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // // 定数用
    // // シェーダーリソースビューと定数バッファービューが連続しており、またレンジも連続しているため、
    // // ルートパラメーター1つに対して[レンジが2つ]という指定を行えば、いっぺんに2つのレジスター設定ができ、切り替えコストも軽減できる
    // rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    // // ディスクリプタレンジのアドレス
    // rootParam[1].DescriptorTable.pDescriptorRanges = &ranges[1];
    // // ディスクリプタレンジ数
    // rootParam[1].DescriptorTable.NumDescriptorRanges = 1;
    // // すべてのシェーダーから見える
    // rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    // サンプラーを作成
    D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = filter;                                                // D3D12_FILTER_MIN_MAG_MIP_LINEAR:線形補間
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                     // 横方向の繰り返し
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                     // 縦方向の繰り返し
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;                     // 奥行き方向の繰り返し
    samplerDesc.MipLODBias = D3D12_DEFAULT_MIP_LOD_BIAS;                        // ??
    samplerDesc.MaxAnisotropy = 1;                                              // ??
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;                   // リサンプリングしない
    samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;      // ボーダーは黒
    samplerDesc.MinLOD = 0.0f;                                                  // ミップマップ最小値
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;                                     // ミップマップ最大値
    samplerDesc.ShaderRegister = 0;                                             // ??
    samplerDesc.RegisterSpace = 0;                                              // ??
    samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;               // ピクセルシェーダーから見える        

    // ルートシグネチャー作成
    // ディスクリプタテーブルをまとめたもの
    // 頂点情報以外のデータをパイプラインの外からシェーダーに送りこむために使われる
    // 今回は頂点だけを流し込んで、それをそのまま表示するため、ルートシグネチャー情報は特に必要ないが
    // 空のルートシグネチャーを設定しないとパイプラインステートは作れません
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};

    // Flagsに「頂点情報（入力アセンブラ）がある」という意味の値を設定
    rootSignatureDesc.NumParameters = 1;            // ルートパラメーター数
    rootSignatureDesc.pParameters = &rootParam;     // ルートパラメーターの先頭アドレス
    rootSignatureDesc.NumStaticSamplers = 1;        
    rootSignatureDesc.pStaticSamplers = &samplerDesc;
    rootSignatureDesc.Flags = ROOT_SIGNATURE_FLAGS;

    HRESULT result = S_OK;

     // バイナリコード作成
    ComPtr<ID3DBlob> rootSigBlob = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    result = D3D12SerializeRootSignature(
                                            &rootSignatureDesc,                   // ルートシグネチャー設定
                                            D3D_ROOT_SIGNATURE_VERSION_1_0,       // ルートシグネチャーバージョン     ※1.1については要調査//ID3D12Device::CheckFeatureSupport()
                                            rootSigBlob.ReleaseAndGetAddressOf(), // シェーダーを作ったときと同じ
                                            errorBlob.ReleaseAndGetAddressOf()    // エラーメッセージ
                                        ); 

    if (FAILED(result))
    {
      if (errorBlob.Get() != nullptr)
      {
        std::string errMsg;
        errMsg.resize(errorBlob->GetBufferSize());
        std::copy_n(static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize(), errMsg.begin());

        ::OutputDebugStringA(errMsg.c_str());
      }
      else
      {
        // result を出力
      }
      assert(false);
      return;
    }

    // ルートシグネチャーオブジェクト作成
    result = device->CreateRootSignature(
                                            0,      // nodemask 0でよい
                                            rootSigBlob->GetBufferPointer(),
                                            rootSigBlob->GetBufferSize(),
                                            IID_PPV_ARGS(m_rootSignature.ReleaseAndGetAddressOf())
                                         );
    
    if(FAILED(result))
    {
      assert(false);
      return;
    }

    rootSigBlob->Release();
    rootSigBlob.Reset();

  }

  void RootSignature::Dispose(void) noexcept
  {
    m_rootSignature.Reset();
  }
}