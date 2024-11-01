struct Output
{
    float4 pos : POSITION;
    float4 svPos : SV_Position;
    float2 uv : TEXCOORD;
};

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);