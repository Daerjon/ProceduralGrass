#pragma once

/*
float3 Position : Position;
float2 Facing : Facing;
float Wind : Wind;
uint Hash : Hash;
uint Type : Type;
float2 ClumpFacing : ClumpFacing;
uint ClumpColor : ClumpColor;
float Height : Height;
float Width : Width;
float Tilt : Tilt;
float Bend : Bend;
uint SideCurve : SideCurve;
*/
#include"dxDevice.h"
#include<DirectXMath.h>

struct inputElement
{
	inputElement(DirectX::XMFLOAT3 Position)
		:Position(Position) {}
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
D3D11_INPUT_ELEMENT_DESC inputLayout[] =
{
	{/*LPCSTR SemanticName*/ "Position",
	/*UINT SemanticIndex*/ 0,
	/*DXGI_FORMAT Format*/ DXGI_FORMAT_R32G32B32_FLOAT,
	/*UINT InputSlot*/ 1,
	/*UINT AlignedByteOffset*/ D3D11_APPEND_ALIGNED_ELEMENT,
	/*D3D11_INPUT_CLASSIFICATION InputSlotClass*/ D3D11_INPUT_PER_INSTANCE_DATA,
	/*UINT InstanceDataStepRate*/ 1},
	{/*LPCSTR SemanticName*/ "Facing",
	/*UINT SemanticIndex*/ 0,
	/*DXGI_FORMAT Format*/ DXGI_FORMAT_R32G32_FLOAT,
	/*UINT InputSlot*/ 1,
	/*UINT AlignedByteOffset*/ D3D11_APPEND_ALIGNED_ELEMENT,
	/*D3D11_INPUT_CLASSIFICATION InputSlotClass*/ D3D11_INPUT_PER_INSTANCE_DATA,
	/*UINT InstanceDataStepRate*/ 1},
	{/*LPCSTR SemanticName*/ "Wind",
	/*UINT SemanticIndex*/ 0,
	/*DXGI_FORMAT Format*/ DXGI_FORMAT_R32_FLOAT,
	/*UINT InputSlot*/ 1,
	/*UINT AlignedByteOffset*/ D3D11_APPEND_ALIGNED_ELEMENT,
	/*D3D11_INPUT_CLASSIFICATION InputSlotClass*/ D3D11_INPUT_PER_INSTANCE_DATA,
	/*UINT InstanceDataStepRate*/ 1},
	{/*LPCSTR SemanticName*/ "Hash",
	/*UINT SemanticIndex*/ 0,
	/*DXGI_FORMAT Format*/ DXGI_FORMAT_R32_UINT,
	/*UINT InputSlot*/ 1,
	/*UINT AlignedByteOffset*/ D3D11_APPEND_ALIGNED_ELEMENT,
	/*D3D11_INPUT_CLASSIFICATION InputSlotClass*/ D3D11_INPUT_PER_INSTANCE_DATA,
	/*UINT InstanceDataStepRate*/ 1},
	{/*LPCSTR SemanticName*/ "Type",
	/*UINT SemanticIndex*/ 0,
	/*DXGI_FORMAT Format*/ DXGI_FORMAT_R32_UINT,
	/*UINT InputSlot*/ 1,
	/*UINT AlignedByteOffset*/ D3D11_APPEND_ALIGNED_ELEMENT,
	/*D3D11_INPUT_CLASSIFICATION InputSlotClass*/ D3D11_INPUT_PER_INSTANCE_DATA,
	/*UINT InstanceDataStepRate*/ 1},
	{/*LPCSTR SemanticName*/ "ClumpFacing",
	/*UINT SemanticIndex*/ 0,
	/*DXGI_FORMAT Format*/ DXGI_FORMAT_R32G32_FLOAT,
	/*UINT InputSlot*/ 1,
	/*UINT AlignedByteOffset*/ D3D11_APPEND_ALIGNED_ELEMENT,
	/*D3D11_INPUT_CLASSIFICATION InputSlotClass*/ D3D11_INPUT_PER_INSTANCE_DATA,
	/*UINT InstanceDataStepRate*/ 1},
	{/*LPCSTR SemanticName*/ "ClumpColor",
	/*UINT SemanticIndex*/ 0,
	/*DXGI_FORMAT Format*/ DXGI_FORMAT_R32_UINT,
	/*UINT InputSlot*/ 1,
	/*UINT AlignedByteOffset*/ D3D11_APPEND_ALIGNED_ELEMENT,
	/*D3D11_INPUT_CLASSIFICATION InputSlotClass*/ D3D11_INPUT_PER_INSTANCE_DATA,
	/*UINT InstanceDataStepRate*/ 1},
	{/*LPCSTR SemanticName*/ "Height",
	/*UINT SemanticIndex*/ 0,
	/*DXGI_FORMAT Format*/ DXGI_FORMAT_R32_FLOAT,
	/*UINT InputSlot*/ 1,
	/*UINT AlignedByteOffset*/ D3D11_APPEND_ALIGNED_ELEMENT,
	/*D3D11_INPUT_CLASSIFICATION InputSlotClass*/ D3D11_INPUT_PER_INSTANCE_DATA,
	/*UINT InstanceDataStepRate*/ 1},
	{/*LPCSTR SemanticName*/ "Width",
	/*UINT SemanticIndex*/ 0,
	/*DXGI_FORMAT Format*/ DXGI_FORMAT_R32_FLOAT,
	/*UINT InputSlot*/ 1,
	/*UINT AlignedByteOffset*/ D3D11_APPEND_ALIGNED_ELEMENT,
	/*D3D11_INPUT_CLASSIFICATION InputSlotClass*/ D3D11_INPUT_PER_INSTANCE_DATA,
	/*UINT InstanceDataStepRate*/ 1},
	{/*LPCSTR SemanticName*/ "Tilt",
	/*UINT SemanticIndex*/ 0,
	/*DXGI_FORMAT Format*/ DXGI_FORMAT_R32_FLOAT,
	/*UINT InputSlot*/ 1,
	/*UINT AlignedByteOffset*/ D3D11_APPEND_ALIGNED_ELEMENT,
	/*D3D11_INPUT_CLASSIFICATION InputSlotClass*/ D3D11_INPUT_PER_INSTANCE_DATA,
	/*UINT InstanceDataStepRate*/ 1},
	{/*LPCSTR SemanticName*/ "Bend",
	/*UINT SemanticIndex*/ 0,
	/*DXGI_FORMAT Format*/ DXGI_FORMAT_R32_FLOAT,
	/*UINT InputSlot*/ 1,
	/*UINT AlignedByteOffset*/ D3D11_APPEND_ALIGNED_ELEMENT,
	/*D3D11_INPUT_CLASSIFICATION InputSlotClass*/ D3D11_INPUT_PER_INSTANCE_DATA,
	/*UINT InstanceDataStepRate*/ 1},
	{/*LPCSTR SemanticName*/ "SideCurve",
	/*UINT SemanticIndex*/ 0,
	/*DXGI_FORMAT Format*/ DXGI_FORMAT_R32_UINT,
	/*UINT InputSlot*/ 1,
	/*UINT AlignedByteOffset*/ D3D11_APPEND_ALIGNED_ELEMENT,
	/*D3D11_INPUT_CLASSIFICATION InputSlotClass*/ D3D11_INPUT_PER_INSTANCE_DATA,
	/*UINT InstanceDataStepRate*/ 1}
};