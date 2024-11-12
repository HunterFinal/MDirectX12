/*

MRenderFramework
Author : MAI ZHICONG

Description : PipelineState Wrapper (Graphics API: DirectX12)

Update History: 2024/11/12 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <Graphics_DX12/PipelineState.h>

#include <Graphics_DX12/ShaderResBlob.h>
#include <d3d12.h>

#include <cassert>

namespace
{
  // 2D
  // 頂点レイアウト作成
  // 渡される頂点データなどをどのように解釈するかをGPUに教えてあげるため
  constexpr D3D12_INPUT_ELEMENT_DESC INPUT_LAYOUT_2D[] = 
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
}

namespace MFramework
{
  PipelineState::PipelineState()
    : m_pipelineState(nullptr)
  {

  }
  PipelineState::~PipelineState()
  {
    Dispose();
  }

  void PipelineState::Init(
                            ID3D12Device* device,
                            ID3D12RootSignature* rootSig,
                            ShaderResBlob* vertexShader,
                            ShaderResBlob* pixelShader,
                            D3D12_BLEND_DESC* blendDesc

                          )
  {
    if (device == nullptr)
    {
      return;
    }

    if (vertexShader == nullptr || vertexShader->Get()->GetBufferPointer() == nullptr)
    {
      return;
    }

    if (pixelShader == nullptr || pixelShader->Get()->GetBufferPointer() == nullptr)
    {
      return;
    }

    // ラスタライザーステートの設定
    D3D12_RASTERIZER_DESC rasDesc = {};
   
    rasDesc.FillMode = D3D12_FILL_MODE_SOLID;         // 中身を塗りつぶす ※ポリゴンの中身、ソリッドモデル
    rasDesc.CullMode = D3D12_CULL_MODE_NONE;          // カリングしない ※背面カリング、PMDモデルには裏側にポリゴンがないものが多い
    rasDesc.FrontCounterClockwise = FALSE;            // false
    rasDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasDesc.DepthClipEnable = TRUE;                   // 深度方向のクリッピングは有効に　※後々必要になってくる
    rasDesc.MultisampleEnable = FALSE;                // まだアンチエイリアシングを使わないためfalse 
    rasDesc.AntialiasedLineEnable = FALSE;
    rasDesc.ForcedSampleCount = 0;
    rasDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

     // パイプラインステート作成
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};

    desc.pRootSignature = rootSig;
    // シェーダーをセット
    desc.VS.pShaderBytecode = vertexShader->Get()->GetBufferPointer();
    desc.VS.BytecodeLength = vertexShader->Get()->GetBufferSize();
    desc.PS.pShaderBytecode = pixelShader->Get()->GetBufferPointer();
    desc.PS.BytecodeLength = pixelShader->Get()->GetBufferSize();

    // サンプルマスクとラスタライザーステート設定
    desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
   
    // ブレンドステート設定
    
    if (blendDesc == nullptr)
    {   
      // BlendOpとLogicOpがあり、メンバーをどう演算するかを表している
      // BlendEnableとLogicOpEnableは演算を行うかを表す変数で、両方同時にtrueになれず、どちらかを選ぶ
      // 今回はブレンドしないため、両方をfalseにする
      D3D12_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
      rtBlendDesc.BlendEnable = FALSE;
      rtBlendDesc.LogicOpEnable = FALSE;
      rtBlendDesc.SrcBlend = D3D12_BLEND_ONE;         // ピクセルシェーダから出力されたRGB値に対して実行するブレンドオプション
      rtBlendDesc.DestBlend = D3D12_BLEND_ZERO;       // 現在のレンダーターゲットのRGB値に対して実行するブレンドオプション
      rtBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;       // SRCとDESTをどのように結合するかを定義するブレンド演算
      rtBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;    // アルファ値に対する実行するブレンドオプション(_COLORで終わるブレンドオプションは設定できない)
      rtBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;  // アルファ値に対する実行するブレンドオプション(_COLORで終わるブレンドオプションは設定できない)
      rtBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;  // SRCAlphaとDESTAlphaをどのように結合するかを定義するブレンド演算
      rtBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;      // レンダーターゲットに対して設定する論理演算を指定する
      // RGBAそれぞれの値を書き込むかどうか指定するための値
      rtBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

      // DX12の魔導書の範囲においては、マルチサンプリング（アンチエイリアシングの適用）でない場合、AlphaToCoverageEnableの値はαテストの有無を表す（？）
      // マルチサンプリング時の「網羅率」まで考慮して描画を行う
      // ピクセルシェーダから出力されたアルファ成分を取得し、マルチサンプリングアンチエイリアス処理を適用する機能
      desc.BlendState.AlphaToCoverageEnable = FALSE;
      // IndependentBlendEnableにtrueを指定すれば、合計8個のレンダーターゲットに対して別々にブレンドステート設定できる。falseだったら0番目が全てに適用する。
      // FALSEに指定すれば、RenderTargetの０が使われ、１～７が無視される
      desc.BlendState.IndependentBlendEnable = FALSE;
      for (UINT i = 0 ; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
      {
        desc.BlendState.RenderTarget[i] = rtBlendDesc;
      }
    }
    else
    {
      desc.BlendState = *blendDesc;
    }
 
    desc.RasterizerState = rasDesc;
    // ??
    desc.DepthStencilState.DepthEnable = false;
    desc.DepthStencilState.StencilEnable = false;
    
    // 入力レイアウトを設定
    desc.InputLayout.pInputElementDescs = INPUT_LAYOUT_2D;
    desc.InputLayout.NumElements = _countof(INPUT_LAYOUT_2D); // 配列の数を取得するためのマクロ

    // IBStripCutValueを設定
    // トライアングルストリップのとき、「切り離せない頂点集合」を特定のインデックスで切り離すための指定を行うもの（詳しくはDX12魔導書P138）
    // 今回特に使わない
    desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED; // カットなし

    // PrimitiveTopologyTypeを設定
    // 構成要素が「点」「線」「三角形」のどれなのかを指定
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // 設定したレンダーターゲットの数だけフォーマットを指定
    desc.NumRenderTargets = 1;                             // 今回レンダーターゲットは一つしかないので1
    desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;  // 0~1に正規化されたRGBA
    desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

    // アンチエイリアシング設定
    // 今回はしない
    desc.SampleDesc.Count = 1;                     // サンプリングは1ピクセルにつき1
    desc.SampleDesc.Quality = 0;                   // クオリティ最低
    // ??
    desc.SampleMask = UINT_MAX;

    HRESULT result = S_OK;

    result = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pipelineState.ReleaseAndGetAddressOf()));

    assert(SUCCEEDED(result));
  }

  void PipelineState::Dispose() noexcept
  {
    m_pipelineState.Reset();
  }

}