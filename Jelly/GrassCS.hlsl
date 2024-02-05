#define MaxX 16
#define MaxY 16
#define MaxIdx MaxX*MaxY

int p[] = { 151,160,137,91,90,15,
   131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
   190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
   88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
   77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
   102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
   135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
   5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
   223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
   129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
   251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
   49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
   138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,151,
   160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69, 142, 8,
   99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
   35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175, 74, 165,
   71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133, 230,220, 105,
   92,41,55,46,245,40,244,102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,
   208, 89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,
   52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,
   16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163, 70,221,153,
   101,155,167, 43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232, 178, 185, 
   112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,
   145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,
   50,45,127, 4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195, 78,
   66,215,61,156,180
   };

float fade(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float t, float a, float b)
{
    return a + t * (b - a);
}

float grad(int hsh, float x, float y, float z)
{
    int h = hsh & 15;
    float u = h < 8 ? x : y,
             v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float noise(float x, float y, float z)
{
    int X = ((int)floor(x)) & 255,
          Y = ((int)floor(y)) & 255,
          Z = ((int)floor(z)) & 255;
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);
    float u = fade(x),
             v = fade(y),
             w = fade(z);
    int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z,
          B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

    return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),
                                     grad(p[BA], x - 1, y, z)),
                             lerp(u, grad(p[AB], x, y - 1, z),
                                     grad(p[BB], x - 1, y - 1, z))),
                     lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1),
                                     grad(p[BA + 1], x - 1, y, z - 1)),
                             lerp(u, grad(p[AB + 1], x, y - 1, z - 1),
                                     grad(p[BB + 1], x - 1, y - 1, z - 1))));
}

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

RWStructuredBuffer<uint> OutCount : register(u1);

groupshared uint valid[MaxIdx];

float uint2float(uint i) //gives float in (0,1)
{
    return float(i) / 4294967295.0;
}

uint hash(float2 fragCoord, uint hash) //gives pseudo-random uint
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

float random(float2 fragCoord, inout uint hsh) //gives pseudo-random float (0,1)
{
    uint seed = hash(fragCoord, hsh);
    hsh = hash(fragCoord, hsh);
    return uint2float(seed);
}

float2 random2(float2 fragCoord, inout uint hsh) //gives pair of pseudo-random float (0,1)
{
    float x = random(fragCoord, hsh);
    float y = random(fragCoord, hsh);
    return float2(x, y);
}

float3 random3(float2 fragCoord, inout uint hsh) //gives triple of pseudo-random float (0,1)
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
    float3 pos = random3(coord, hsh);
    OutBuff[idx].Positon = pos;
    OutBuff[idx].Facing = random2(coord, hsh);
    OutBuff[idx].Wind = noise(pos.x, pos.y, pos.z);
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
    if(idx == 0)
        OutCount[0] = valid[0];
}