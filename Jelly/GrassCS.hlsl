#define MaxX 16
#define MaxY 16
#define MaxIdx MaxX*MaxY


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

//RWStructuredBuffer<uint> OutCount : register(u1);

groupshared uint valid[MaxIdx];

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

float random(float2 fragCoord, inout uint hsh)
{
    uint seed = hash(fragCoord, hsh);
    hsh = hash(fragCoord, hsh);
    return uint2float(seed);
}

float2 random2(float2 fragCoord, inout uint hsh)
{
    float x = random(fragCoord, hsh);
    float y = random(fragCoord, hsh);
    return float2(x, y);
}

float3 random3(float2 fragCoord, inout uint hsh)
{
    float x = random(fragCoord, hsh);
    float y = random(fragCoord, hsh);
    float z = random(fragCoord, hsh);
    return float3(x, y, z);
}

[numthreads(MaxX, MaxY, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint idx = MaxY * DTid.x + DTid.y;
    float2 coord = { uint2float(DTid.x), uint2float(DTid.y) };
    uint hsh = hash(coord, 0);
    OutBuff[idx].Hash = hsh;
    OutBuff[idx].Positon = random3(coord, hsh);
    OutBuff[idx].Facing = random2(coord, hsh);
    OutBuff[idx].Wind = random(coord, hsh);
    OutBuff[idx].Height = random(coord, hsh);
    OutBuff[idx].Width = random(coord, hsh);
    OutBuff[idx].Tilt = random(coord, hsh);
    OutBuff[idx].Bend = random(coord, hsh);
    OutBuff[idx].ClumpFacing = random2(coord, hsh);
    OutBuff[idx].Type = hash(coord, hsh);
    hsh = hash(coord, hsh);
    OutBuff[idx].SideCurve = hash(coord, hsh);
    hsh = hash(coord, hsh);
    OutBuff[idx].ClumpColor = hash(coord, hsh);
    valid[idx] = 1;
    for (int i = 1; i < 512; i<<=2)
    {
        AllMemoryBarrierWithGroupSync();
        if (idx % (i<<2) == 0)
        {
            valid[idx] += valid[idx + i];
            valid[idx] += valid[idx + 2 * i];
            valid[idx] += valid[idx + 3 * i];
        }
    }
    //if(idx == 0)
    //    OutCount[0] = valid[0];
}