#pragma once
#include "dxApplication.h"
#include "renderPass.h"
#include "inputLayoutManager.h"
#include "camera.h"
#include "viewFrustrum.h"
#include "guiRenderer.h"
#include "modelLoader.h"

namespace mini
{
	class WindowsApp : public directx::dx_app
	{
	public:
		WindowsApp(HINSTANCE hInst);

	protected:

		[[nodiscard]] int main_loop() override;

		void update(utils::clock const& clock) override;
		void render() override;

		std::optional<LRESULT> process_message(windows::message const& msg) override;

		size_t addModelFromFile(const std::string& path);
		std::tuple<size_t, ModelDescription> addModelFromTxtFile(const std::string& path);
		size_t addModelFromString(const std::string& model, bool smoothNormals = true);
		size_t addModelFromBuffers(std::vector<float> vert, std::vector<float> norm, std::vector<unsigned short> ind);
		size_t addModelParticleSystem();

		size_t addPass(const std::wstring& vsShader, const std::wstring& psShader);
		size_t addPass(const std::wstring& vsShader, const std::wstring& gsShader, const std::wstring& psShader);
		size_t addPass(const std::wstring& vsShader, const std::wstring& psShader, const std::string& renderTarget,
			bool clearRenderTarget = false);
		size_t addPass(const std::wstring& vsShader, const std::wstring& psShader,
			const RenderTargetsEffect& renderTarget, bool clearRenderTarget = false);
		void addRasterizerState(size_t passId, const directx::rasterizer_info& desc);

		Model& model(size_t modelId) { return *m_models[modelId]; }
		const Model& model(size_t modelId) const { return *m_models[modelId]; }

		RenderPass& pass(size_t passId) { return m_passes[passId]; }
		const RenderPass& pass(size_t passId) const { return m_passes[passId]; }

		void addModelToPass(size_t passId, size_t modelId);

		void copyRenderTarget(size_t passId, std::string dstTexture);
		void copyDepthBuffer(size_t passId, std::string dstTexture);

		CBVariableManager m_variables;

		static constexpr float ROTATION_SPEED = 0.01f;
		static constexpr float ZOOM_SPEED = 0.02f;

		ModelLoader m_loader;
		std::vector<std::unique_ptr<Model>> m_models;
		std::vector<RenderPass> m_passes;
		InputLayoutManager m_layouts;
		directx::fps_camera m_camera;
		ViewFrustrum m_frustrum;
		GUIRenderer m_gui;
	};

}
