#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;
struct ModelDescription
{
	std::vector<XMFLOAT3> pos;
	std::vector<std::pair<int,XMFLOAT3>> vert;
	std::vector<std::tuple<int, int, int>> triangle;
	std::vector<std::tuple<int, int, int, int>> edges;
};

