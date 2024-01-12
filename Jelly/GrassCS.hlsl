struct Blade
{
    float3 Positon;
    float2 Facing;
    float Wind;
    uint Hash;
    uint Type;
    float2 ClumpFacing;
    uint ClumpColor;
    float Height;
    float Width;
    float Tilt;
    float Bend;
    uint SideCurve;
};

RWStructuredBuffer<Blade> OutBuff : register(u0);

float uint2float(uint i)
{
    return float(i) / 4294967295.0;
}

uint hash(float2 fragCoord, uint hash)
{
    uint seed = uint(fragCoord.y * 512 + fragCoord.x) + hash;
    seed ^= 2747636419u;
    seed *= 2654435769u;
    seed ^= seed >> 16;
    seed *= 2654435769u;
    seed ^= seed >> 16;
    seed *= 2654435769u;
    return seed;
}

float random(float2 fragCoord, inout uint hash)
{
    uint seed = hash(fragCoord, hash);
    hash = hash(fragCoord, hash);
    return uint2float(seed);
}

float2 random2(float2 fragCoord, inout uint hash)
{
    float x = random(fragCoord, hash);
    float y = random(fragCoord, hash);
    return float2(x, y);
}

float3 random3(float2 fragCoord, inout uint hash)
{
    float x = random(fragCoord, hash);
    float y = random(fragCoord, hash);
    float z = random(fragCoord, hash);
    return float3(x, y, z);
}

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint idx = DTid.x;
    float2 coord = { uint2float(DTid.x), uint2float(DTid.y) };
    uint hash = hash(coord, 0);
    OutBuff[idx].Hash = hash;
    OutBuff[idx].Positon = random3(coord, hash);
    OutBuff[idx].Facing = random2(coord, hash);
    OutBuff[idx].Wind = random(coord, hash);
    OutBuff[idx].Height = random(coord, hash);
    OutBuff[idx].Width = random(coord, hash);
    OutBuff[idx].Tilt = random(coord, hash);
    OutBuff[idx].Bend = random(coord, hash);
    OutBuff[idx].ClumpFacing = random2(coord, hash);
    OutBuff[idx].Type = hash(coord, hash);
    hash = hash(coord, hash);
    OutBuff[idx].SideCurve = hash(coord, hash);
    hash = hash(coord, hash);
    OutBuff[idx].ClumpColor = hash(coord, hash);
}