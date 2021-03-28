#include "includes.h"

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 20, NULL, io.Fonts->GetGlyphRangesCyrillic());
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;
bool openmenu = true;
static int tabb = 3;

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags, HMODULE hMod)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	auto flags = ImGuiConfigFlags_NoMouseCursorChange | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;

	// Открытие Меню
	if (GetAsyncKeyState(VK_HOME) & 1) {

		openmenu = !openmenu;
	}

	// Отключить чит
	if (GetAsyncKeyState(VK_END)) {

		kiero::shutdown();
	}

	if (openmenu) {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin(xorstr(u8"RustD3D11"), nullptr, flags);
		
		// Таблицы
		if (ImGui::Button(xorstr(u8"AimBot"), ImVec2(100.f, 0.f)))
			tabb = 0;
		ImGui::SameLine(0.f, 2.f);
		if (ImGui::Button(xorstr(u8"Vusuals"), ImVec2(100.f, 0.f)))
			tabb = 1;
		ImGui::SameLine(0.f, 2.f);
		if (ImGui::Button(xorstr(u8"Misc"), ImVec2(100.f, 0.f)))
			tabb = 2;
		ImGui::SameLine(0.f, 2.f);
		if (ImGui::Button(xorstr(u8"Info"), ImVec2(100.f, 0.f)))
			tabb = 3;
		// Таблицы

		// Таблицы ЧекБоксы
		if (tabb == 0) {
			ImGui::Checkbox(xorstr(u8"Rage"), &Vars::Aimbot::rage);
			ImGui::Checkbox(xorstr(u8"Legit"), &Vars::Aimbot::legit);
			ImGui::Checkbox(xorstr(u8"Silent Aim"), &Vars::Aimbot::silentaim);
		}
		else if (tabb == 1) {
			ImGui::Checkbox(xorstr(u8"Esp"), &Vars::Visuals::esp);
			ImGui::Checkbox(xorstr(u8"Draw Healthbar"), &Vars::Visuals::healthbar);
			ImGui::Checkbox(xorstr(u8"Draw Distance"), &Vars::Visuals::distance);
		}
		else if (tabb == 2) {
			ImGui::Checkbox(xorstr(u8"Человек Паук"), &Vars::Misc::spiderman);
			ImGui::Checkbox(xorstr(u8"Фейковые Лаги"), &Vars::Misc::fakelag);
			ImGui::Checkbox(xorstr(u8"Нет Отдачи Оружия"), &Vars::Misc::norecoil);
		}
		else if (tabb == 3) {
			ImGui::Text(xorstr(u8"Разработчик меню: Darwin"));
			ImGui::Text(xorstr(u8"Кнопка меню: Home | Выключить все: End"));
		}
		// Таблицы ЧекБоксы

		ImGui::End();
		ImGui::Render();

		pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	return oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)&oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hMod);
		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, hMod, 0, 0));
	}
	return TRUE;
}