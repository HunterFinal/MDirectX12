struct Input
{
    float4 pos : POSITION;
    float4 svPos : SV_Position;
};

float4 BasicPS(Input input) : SV_TARGET
{
    return float4((float2(0.0f, 1.0f) + input.pos.xy) * 0.5f, 1.0f, 1.0f);
}