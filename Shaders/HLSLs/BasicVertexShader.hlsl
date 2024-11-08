#include "BasicShaderHeader.hlsli"

Output BasicVS(float4 pos : Position, float2 uv: TEXCOORD)
{
    Output o;
    o.pos = pos;
    o.svPos = mul(m.mat, pos);  // 列優先であるため、左方向にかけていきます
    o.uv = uv;
    
    return o;
}