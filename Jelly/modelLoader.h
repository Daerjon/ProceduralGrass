#pragma once

#include "dxDevice.h"
#include "model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "ModelDescription.h"

namespace mini {
	class InputLayoutManager;
}

struct aiNode;
struct aiMesh;
namespace Assimp
{
	class Importer;
}

namespace mini
{
	namespace Jelly
	{
		class ModelLoader
		{
		public:
			ModelLoader(const DxDevice& device)
				: m_device(device) { }

			Model LoadFromFile(const std::string& filename, InputLayoutManager& layouts, bool smoothNormals = true);
			std::tuple<Model, ModelDescription> loadFromTxtFile(const std::string& filename, InputLayoutManager& layouts);
			//Creates model using extended NFF syntax
			Model LoadFromString(const std::string& modelDescription, InputLayoutManager& layouts, bool smoothNormals = true);
			Model CreateModel(const std::vector<float>& vert, const std::vector<float>& norm, const std::vector<unsigned short>& inds, InputLayoutManager& layouts);
			Mesh CreateMesh(const std::vector<float>& vert, const std::vector<float>& norm, const std::vector<unsigned short>& inds, InputLayoutManager& layouts);

		private:
			static void initLoader(Assimp::Importer& importer);
			static const aiScene* readFromFile(const std::string& filename, Assimp::Importer& importer, bool smoothNormals);
			static const aiScene* readFromMemory(const std::string& buffer, Assimp::Importer& importer, bool smoothNormals);
			Model convertToModel(const aiScene* scene, InputLayoutManager& layouts);
			static int* addNode(std::vector<ModelNode>& nodes, aiNode* pAINode);

			DxDevice m_device;
		};
	}
}