struct Output
{
    float4 pos : POSITION;
    float4 svPos : SV_Position;
    float2 uv : TEXCOORD;
};

struct CMatrix
{
    matrix mat; // 変換行列
};

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);
// Shader Model 5.1以降
ConstantBuffer<CMatrix> m : register(b0); // 定数バッファー

//cbuffer cbuff0 : register(b0) // 定数バッファー
//{
//    matrix mat; // 変換行列
//}
