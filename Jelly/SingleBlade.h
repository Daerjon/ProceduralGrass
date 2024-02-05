#pragma once
#define MaxBladesX 16
#define MaxBladesY 16
#define MaxBlades MaxBladesX*MaxBladesY
#include"myDxApplication.h"

using namespace mini::directx;
namespace mini::Jelly
{
    struct SingleBladeSt
    {
        DirectX::XMFLOAT3 Positon;
        DirectX::XMFLOAT2 Facing;
        float Wind;
        unsigned int Hash;
        unsigned int Type;
        DirectX::XMFLOAT2 ClumpFacing;
        unsigned int ClumpColor;
        float Height;
        float Width;
        float Tilt;
        float Bend;
        unsigned int SideCurve;
    };

    HRESULT CreateBufferUAV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut)
    {
        D3D11_BUFFER_DESC descBuf = {};
        pBuffer->GetDesc(&descBuf);

        D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
        desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        desc.Buffer.FirstElement = 0;

        if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
        {
            // This is a Raw Buffer

            desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
            desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
            desc.Buffer.NumElements = descBuf.ByteWidth / 4;
        }
        else
            if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
            {
                // This is a Structured Buffer

                desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
                desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
            }
            else
            {
                return E_INVALIDARG;
            }

        return pDevice->CreateUnorderedAccessView(pBuffer, &desc, ppUAVOut);
    }

}