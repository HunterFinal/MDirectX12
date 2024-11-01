#include "BasicShaderHeader.hlsli"

Output BasicVS(float4 pos : Position, float2 uv: TEXCOORD)
{
    Output o;
    o.pos = pos;
    o.svPos = pos;
    o.uv = uv;
    
    return o;
}