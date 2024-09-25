struct Output
{
    float4 pos : POSITION;
    float4 svPos : SV_Position;
};
Output BasicVS(float4 pos : Position)
{
    Output o;
    o.pos = pos;
    o.svPos = pos;
    return o;
}