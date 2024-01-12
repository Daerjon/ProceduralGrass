#pragma once

#include "dxApplication.h"

namespace mini::Jelly
{
	class MyDxApplication : public mini::directx::dx_app
	{
	public:
		MyDxApplication(HINSTANCE instance,
			int window_width = s_default_window_width,
			int window_height = s_default_window_height,
			std::wstring_view window_title = s_window_title);
		~MyDxApplication();

		int main_loop() override;
		std::optional<LRESULT> process_message(mini::windows::message const& msg) override;
	};
}