#pragma once
#define MaxBladesX 256

#define MaxBladesY 1
#define MaxBlades MaxBladesX*MaxBladesY
#include"myDxApplication.h"

using namespace mini::directx;
namespace mini::Jelly
{
    struct SingleBladeSt
    {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT2 Facing;
        float Wind;
        uint32_t Hash;
        uint32_t Type;
        DirectX::XMFLOAT2 ClumpFacing;
        uint32_t ClumpColor;
        float Height;
        float Width;
        float Tilt;
        float Bend;
        uint32_t SideCurve;
    };

    HRESULT CreateBufferUAV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut)
    {
        D3D11_BUFFER_DESC descBuf = {};
        pBuffer->GetDesc(&descBuf);

        D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
        desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        desc.Buffer.FirstElement = 0;

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

    HRESULT CreateBufferSRV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut)
    {
        D3D11_BUFFER_DESC descBuf = {};
        pBuffer->GetDesc(&descBuf);

        D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
        desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
        desc.Buffer.FirstElement = 0;

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

        return pDevice->CreateShaderResourceView(pBuffer, &desc, ppSRVOut);
    }
}