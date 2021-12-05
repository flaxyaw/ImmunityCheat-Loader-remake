/*Author: JustMirii
* 
* Date: US Date: 12.4.2021   EU Date: 4.12.2021
* 
* Contact:
* 
* Discord: Iam a flax fanboy#8973
* 
* Website: https://iamundetected.github.io/
* 
* Github: https://github.com/IamUndetected/
*/

/*
* THIS IS NOT THE REAL immunity.digital cheat!
* 
* This is just a Loader menu remake with KeyAuth.com system
* 
* if you want you can checkout the real cheat but i dont know if you want to
* 
* 
* Credits for the real Immunity: 
* 
* Luca.
* MrAnonymous
* un1verse
* Wesh
*/

/*
* 
* Credits for this remake:
* 
* https://github.com/keyauth
* 
* https://github.com/Lolyss1337/KeyAuth-ImGui-Example
* 
* https://github.com/ocornut/imgui
* 
* and me c:
*/

//*Please dont use it for bad or sell it but enjoy using it as a base or something
#include <iostream>
#include "api/KeyAuth.hpp"
#include "xorstr.hpp"
#include <tlhelp32.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <windows.h>
#include <thread>
#include <random>

#include <d3d9.h>
#pragma comment(lib,"d3d9.lib")

#pragma comment(lib, "D3dx9")

IDirect3DTexture9* masterlogo;

#include "others/ImGui/imgui.h"
#include "others/ImGui/imgui_impl_dx9.h"
#include "others/ImGui/imgui_impl_win32.h"


using namespace KeyAuth;


std::string name = XorStr(""); //Application name found in application settings
std::string ownerid = XorStr(""); //Owner ID Found in user settings
std::string secret = XorStr(""); //Application secret found in Application settings
std::string version = XorStr("1.0"); // Version can be changed but is not really important

api KeyAuthApp(name, ownerid, secret, version);

bool loader_active = true;

static int width = 350;
static int height = 200;

char PassWord[20] = "";
char Licence[50] = "";
char UserName[20] = "";
char RgPassWord[20] = "";
char RgUserName[20] = "";

HWND main_hwnd = nullptr;

LPDIRECT3DDEVICE9        g_pd3dDevice;
D3DPRESENT_PARAMETERS    g_d3dpp;
LPDIRECT3D9              g_pD3D;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

bool CreateDeviceD3D(HWND hWnd)
{
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;

	// Create the D3DDevice
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
	//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;
	return true;
}

void CleanupDeviceD3D()
{
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

bool LoginCheck = false;

void InfLog()
{
	RECT screen_rect;
	GetWindowRect(GetDesktopWindow(), &screen_rect);
	auto x = float(screen_rect.right - width) / 2.f;
	auto y = float(screen_rect.bottom - height) / 2.f;

	ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(200, 70));
	ImGui::Begin("More User info!", &loader_active, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar);
	{
		ImGui::Columns(1, NULL, true);
		{
			ImGui::Text("Username : %s", KeyAuthApp.user_data.username);

			//This my cause an error if yes just add #define _CRT_SECURE_NO_WARNINGS under the includes or search it up on google

			time_t currtime;
			struct tm* tminfo;
			time(&currtime);
			tminfo = localtime(&currtime);

			std::time_t x = std::mktime(tminfo);
			std::time_t y = std::mktime(&KeyAuthApp.user_data.expiry);
			if (x != (std::time_t)(-1) && y != (std::time_t)(-1))
			{
				double difference = std::difftime(y, x) / (60 * 60 * 24);
				ImGui::Text("%f day(s) left", difference);
			}

			ImGui::EndChild();
		}
	}
	ImGui::End();
}

int APIENTRY WindownsMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL,L"Loaders", NULL };
	RegisterClassEx(&wc);
	main_hwnd = CreateWindow(wc.lpszClassName, L"Loaders", WS_POPUP, 0, 0, 5, 5, NULL, NULL, wc.hInstance, NULL);

	if (!CreateDeviceD3D(main_hwnd)) {
		CleanupDeviceD3D();
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}
	ShowWindow(main_hwnd, SW_HIDE);
	UpdateWindow(main_hwnd);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	constexpr auto ColorFromBytes = [](uint8_t r, uint8_t g, uint8_t b)
	{
		return ImVec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 1.0f);
	};

	auto& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;


	/*
	* 
	* Things to add/change:
	* 
	* Change color of the textbox to 87, 87, 87
	* 
	* Change the alignment a little bit
	*/
	
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.09f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.36f, 0.36f, 0.36f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.61f, 0.61f, 0.59f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	style.WindowRounding = 0.0f;
	style.ChildRounding = 0.0f;
	style.FrameRounding = 2.0f;
	style.GrabRounding = 0.0f;
	style.PopupRounding = 0.0f;
	style.ScrollbarRounding = 0.0f;
	style.TabRounding = 0.0f;
	style.FramePadding.y = 5.0f;
	style.ButtonTextAlign = ImVec2(0.5, 0.5);


	ImGui_ImplWin32_Init(main_hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);

	DWORD window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize;
	RECT screen_rect;
	GetWindowRect(GetDesktopWindow(), &screen_rect);
	auto x = float(screen_rect.right - width) / 2.f;
	auto y = float(screen_rect.bottom - height) / 2.f;

	static int Tabs = 2;

	bool InfWindow = false;

	KeyAuthApp.init();

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT && !LoginCheck)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Once);
			ImGui::SetNextWindowSize({ 401.f,292.f });
			ImGui::SetNextWindowBgAlpha(1.0f);


			//Please make the Register via KeyAuth website because the real immunity does not have a register in the loader :=)
			ImGui::Begin("Immunity Login", &loader_active, window_flags);
			{

				if (Tabs == 2)
				{
					ImGui::Text("Please Login");

					ImGui::Spacing();

					ImGui::Text("Username:");

					ImGui::Spacing();

					ImGui::InputText(" ", UserName, IM_ARRAYSIZE(UserName));

					ImGui::Spacing();

					ImGui::Text("Password:");

					ImGui::Spacing();

					ImGui::InputText("", PassWord, IM_ARRAYSIZE(PassWord), ImGuiInputTextFlags_Password);

					ImGui::Spacing();
					ImGui::Spacing();


					if(ImGui::Button("Login##Log", ImVec2(201, 27)))
					{
						KeyAuthApp.login(UserName, PassWord);

						MessageBox(NULL, TEXT("Login successful!"),TEXT("Login"), MB_OK);

						Tabs = 2;

						InfWindow = true;
					}

					//Maybe add it as a clickable link like the real one
					
					ImGui::SetCursorPos({ 267.f, 270.f });
					ImGui::Text("immunity.digital");
					
				}
			}
			ImGui::End();

			if (InfWindow)
			{
				InfLog();
			}
		}
		ImGui::EndFrame();

		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
			ResetDevice();
		}
		if (!loader_active) {
			msg.message = WM_QUIT;
		}
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	CleanupDeviceD3D();
//	DestroyWindow(main_hwnd);
	UnregisterClass(wc.lpszClassName, wc.hInstance);
//	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR lpCmdLine,int nCmdShow)
{

//	SetConsoleTitleA(XorStr("Loader").c_str());
//	std::cout << XorStr("\n\n Connecting..");

	WindownsMain(0, 0, 0, 0);

//	MessageBox(NULL, TEXT("Login successful! %d"), TEXT("Login"), MB_OK);

	//KeyAuthApp.init();
	//system(XorStr("cls").c_str());
	//
	//std::cout << XorStr("\n\n [1] Login\n [2] Register\n [3] Upgrade\n [4] License key only\n\n Choose option: ");

	//int option;
	//std::string username;
	//std::string password;
	//std::string key;

	//std::cin >> option;
	//switch (option)
	//{
	//	case 1:
	//		std::cout << XorStr("\n\n Enter username: ");
	//		std::cin >> username;
	//		std::cout << XorStr("\n Enter password: ");
	//		std::cin >> password;
	//		KeyAuthApp.login(username, password);
	//		break;
	//	case 2:
	//		std::cout << XorStr("\n\n Enter username: ");
	//		std::cin >> username;
	//		std::cout << XorStr("\n Enter password: ");
	//		std::cin >> password;
	//		std::cout << XorStr("\n Enter license: ");
	//		std::cin >> key;
	//		KeyAuthApp.regstr(username,password,key);
	//		break;
	//	case 3:
	//		std::cout << XorStr("\n\n Enter username: ");
	//		std::cin >> username;
	//		std::cout << XorStr("\n Enter license: ");
	//		std::cin >> key;
	//		KeyAuthApp.upgrade(username, key);
	//		break;
	//	case 4:
	//		std::cout << XorStr("\n Enter license: ");
	//		std::cin >> key;
	//		KeyAuthApp.license(key);
	//		break;
	//	default:
	//		std::cout << XorStr("\n\n Status: Failure: Invalid Selection");
	//		Sleep(3000);
	//		exit(0);
	//}
	
	/*
	// download file
    std::vector<std::uint8_t> bytes = KeyAuthApp.download("123456");
    std::ofstream file("file.exe", std::ios_base::out | std::ios_base::binary);
    file.write((char*)bytes.data(), bytes.size());
    file.close();
	*/
	
	// KeyAuthApp.log("user logged in"); // send event to logs. if you set discord webhook in app settings, it will send there too
	// KeyAuthApp.webhook("HDb5HiwOSM", "&type=black&ip=1.1.1.1&hwid=abc"); // webhook request to securely send GET request to API, here's what it looks like on dashboard https://i.imgur.com/jW74Hwe.png
	// KeyAuthApp.ban(); // ban the current user, must be logged in

	//#pragma region
	//time_t rawtime = mktime(&KeyAuthApp.user_data.expiry);
	//struct tm* timeinfo;
	//timeinfo = localtime(&rawtime);
	//printf(XorStr("\n Your Subscription Expires At: %s").c_str(), asctime(timeinfo));
	//
	//time_t currtime;
	//struct tm* tminfo;
	//time(&currtime);
	//tminfo = localtime(&currtime);

	//std::time_t x = std::mktime(tminfo);
	//std::time_t y = std::mktime(&KeyAuthApp.user_data.expiry);
	//if (x != (std::time_t)(-1) && y != (std::time_t)(-1))
	//{
	//	double difference = std::difftime(y, x) / (60 * 60 * 24);
	//	std::cout << "\n " << difference << " day(s) left" << std::endl;
	//}
	//#pragma endregion Display Expiration Date and Days Left Until Expiry
	//
	//Sleep(-1); // this is to keep your application open for test purposes. it pauses your application forever, remove this when you want.
}
