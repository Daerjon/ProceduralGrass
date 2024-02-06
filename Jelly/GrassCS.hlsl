#pragma target 5.1
#define MaxX 768
#define MaxY 1
#define MaxIdx MaxX*MaxY
#define GroupsX 20
#define GroupsY 20
#define PatchSize 5

cbuffer Time : register(b0)
{
    float4 time;
}

cbuffer Group : register(b1)
{
    int4 group;
}

float3 mod289(float3 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float4 mod289(float4 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float4 permute(float4 x)
{
    return mod289(((x * 34.0) + 10.0) * x);
}

float4 taylorInvSqrt(float4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(float3 v)
{
    const float2 C = float2(1.0 / 6.0, 1.0 / 3.0);
    const float4 D = float4(0.0, 0.5, 1.0, 2.0);
    float3 i = floor(v + dot(v, C.yyy));
    float3 x0 = v - i + dot(i, C.xxx);
    float3 g = step(x0.yzx, x0.xyz);
    float3 l = 1.0 - g;
    float3 i1 = min(g.xyz, l.zxy);
    float3 i2 = max(g.xyz, l.zxy);
    float3 x1 = x0 - i1 + C.xxx;
    float3 x2 = x0 - i2 + C.yyy;
    float3 x3 = x0 - D.yyy;
    i = mod289(i);
    float4 p = permute(permute(permute(
             i.z + float4(0.0, i1.z, i2.z, 1.0))
           + i.y + float4(0.0, i1.y, i2.y, 1.0))
           + i.x + float4(0.0, i1.x, i2.x, 1.0));
    float n_ = 0.142857142857;
    float3 ns = n_ * D.wyz - D.xzx;
    float4 j = p - 49.0 * floor(p * ns.z * ns.z);
    float4 x_ = floor(j * ns.z);
    float4 y_ = floor(j - 7.0 * x_);
    float4 x = x_ * ns.x + ns.yyyy;
    float4 y = y_ * ns.x + ns.yyyy;
    float4 h = 1.0 - abs(x) - abs(y);
    float4 b0 = float4(x.xy, y.xy);
    float4 b1 = float4(x.zw, y.zw);
    float4 s0 = floor(b0) * 2.0 + 1.0;
    float4 s1 = floor(b1) * 2.0 + 1.0;
    float4 sh = -step(h, float4(0.0,0.0,0.0,0.0));
    float4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
    float4 a1 = b1.xzyw + s1.xzyw * sh.zzww;
    float3 p0 = float3(a0.xy, h.x);
    float3 p1 = float3(a0.zw, h.y);
    float3 p2 = float3(a1.xy, h.z);
    float3 p3 = float3(a1.zw, h.w);
    float4 norm = taylorInvSqrt(float4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;
    float4 m = max(0.5 - float4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0);
    m = m * m;
    return 105.0 * dot(m * m, float4(dot(p0, x0), dot(p1, x1),
                                dot(p2, x2), dot(p3, x3)));
}

struct Blade
{
    float3 Positon;
    float2 Facing;
    float Wind;
    uint Hash;
    uint Type;
    int2 ClumpFacing;
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

float uint2float(uint i) //gives float in (0,1)
{
    return float(i) / 4294967295.0;
}

uint hash(uint num, uint hash) //gives pseudo-random uint
{
    uint seed = num + hash;
    seed ^= 2747636419u;
    seed *= 2654435769u;
    seed ^= seed >> 16;
    seed *= 2654435769u;
    seed ^= seed >> 16;
    seed *= 2654435769u;
    return seed;
}

float random(uint num, inout uint hsh) //gives pseudo-random float (0,1)
{
    uint seed = hash(num, hsh);
    hsh = hash(num, hsh);
    return uint2float(seed);
}

float random(uint num) //gives pseudo-random float (0,1)
{
    uint seed = hash(num, 0);
    return uint2float(seed);
}

float2 random2(uint num, inout uint hsh) //gives pair of pseudo-random float (0,1)
{
    float x = random(num, hsh);
    float y = random(num, hsh);
    return float2(x, y);
}

float2 random2(uint num) //gives pair of pseudo-random float (0,1)
{
    float x = random(num);
    float y = random(num);
    return float2(x, y);
}

float3 random3(uint num, inout uint hsh) //gives triple of pseudo-random float (0,1)
{
    float x = random(num, hsh);
    float y = 0.f;
    float z = random(num, hsh);
    return float3(x, y, z);
}

int2 getClump(float3 pos, out float2 clumpPos)
{
    float clumpVar = 0.7;
    
    float mind = 100000.0f;
    float min2d = 9999.0f;
    int2 groupOffset = float2(-1, -1);
    int2 groupOffset2 = float2(-1, -1);
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++)
        {
            float2 cpos = (float2(float(group.x) + float(i), float(group.y) + float(j))
            + clumpVar * random2(GroupsY * (group.x + i) + (group.y + j))
            ) * 5;
            float2 vec = cpos - pos.xz;
            float clump = dot(vec, vec);
            if (clump < mind)
            {
                min2d = mind;
                mind = clump;
                groupOffset2 = groupOffset;
                groupOffset = float2(i, j);
                clumpPos = cpos;

            }
            else if (clump < min2d)
            {
                min2d = clump;
                groupOffset2 = float2(i, j);
            }
        }
    return groupOffset + group.xy;
}

[numthreads(MaxX, MaxY, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint groupIdx = hash(20 * group.x + group.y, 0);
    uint idx = MaxY * DTid.x + DTid.y;
    uint hsh = hash(idx, groupIdx);
    float3 Gpos = float3(group.x /*+ ((group.y % 2 == 0) ? 0.5f : 0.0f)*/, 0.0f, group.y);
    OutBuff[idx].Hash = hsh;
    float3 pos = (Gpos + random3(idx, hsh)) * 5;
    float2 clumpPosition;
    int2 clump = getClump(pos, clumpPosition);
    uint clumpIdx = hash(20 * clump.x + clump.y, 0);
    uint chsh = hash(clumpIdx, 0);
    float clumpHeight = random(clumpIdx, chsh);
    float clumpWidth = random(clumpIdx, chsh);
    OutBuff[idx].Positon = pos;
    OutBuff[idx].Facing = normalize(2.0f * random2(idx, hsh) - 1.0f);
    OutBuff[idx].Wind = snoise(float3((pos.x + time.x), time.x, pos.z) / 7.0f);
    OutBuff[idx].Height = (random(idx, hsh) + random(idx, hsh) + 3 * clumpHeight) / 5.0f;
    OutBuff[idx].Width = random(idx, hsh) * 0.2f + 0.8f * random(clumpIdx, chsh);
    OutBuff[idx].Tilt = random(idx, hsh) * 0.3f + 0.7f * random(clumpIdx, chsh);
    OutBuff[idx].Bend = random(idx, hsh);
    OutBuff[idx].ClumpFacing = normalize(clumpPosition - pos.xz);
    OutBuff[idx].Type = hash(clumpIdx, chsh);
    hsh = hash(idx, hsh);
    OutBuff[idx].SideCurve = hash(idx, hsh);
    hsh = hash(idx, hsh);
    OutBuff[idx].ClumpColor = hash(clumpIdx, chsh) % 4;
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