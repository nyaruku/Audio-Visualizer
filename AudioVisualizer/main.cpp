#pragma once
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#pragma execution_character_set("utf-8")

#pragma region Stuff
#include <d3d9.h>
#include <d3dx9.h>
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>
#include <tchar.h>
#include <vector>
#include <filesystem>
#include <locale>
#include <codecvt>
#include <Windows.h>
#include <string>
#include <utility>
#include "bass.h"
#include <iostream>
#include <iomanip>

// Data
static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};
HWND hwnd;
LPD3DXLINE dxLine = NULL; // Declare the line object
LPD3DXLINE dxLine2 = NULL; // Declare the line object
// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();

double GetHzAtPos(float i_sample_rate, int i_true_numLines, int i_numLinesDyn, int i_workspace_x, int pos_x) {
	return ((float)((float)(((i_sample_rate / 2) / (i_true_numLines)) * i_numLinesDyn) / i_workspace_x) * pos_x);


}
int GetHzAtArrayPos(float i_sample_rate, int i_numLinesDyn, int pos) {
	return (int)(((float)((i_sample_rate / 2) / i_numLinesDyn)) * pos);
}
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#pragma endregion
std::string convertSecondsToTime(double seconds) {
	int hours = static_cast<int>(seconds / 3600);
	seconds -= hours * 3600;
	int minutes = static_cast<int>(seconds / 60);
	seconds -= minutes * 60;
	int secs = static_cast<int>(seconds);
	int milliseconds = static_cast<int>((seconds - secs) * 1000);

	std::stringstream ss;
	ss << std::setw(2) << std::setfill('0') << hours << ":"
		<< std::setw(2) << std::setfill('0') << minutes << ":"
		<< std::setw(2) << std::setfill('0') << secs << ":"
		<< std::setw(3) << std::setfill('0') << milliseconds;

	return ss.str();
}
void DrawLine(LPDIRECT3DDEVICE9 pDevice, float startX, float startY, float endX, float endY, float thickness, D3DCOLOR color)
{
	// Create a line
	LPD3DXLINE pLine;
	D3DXCreateLine(pDevice, &pLine);

	// Set line width
	pLine->SetWidth(thickness);

	// Set line pattern (rounded)
	pLine->SetGLLines(TRUE);

	// Define the line points
	D3DXVECTOR2 line[2];
	line[0] = D3DXVECTOR2(startX, startY);
	line[1] = D3DXVECTOR2(endX, endY);

	// Draw the line
	pLine->Begin();
	pLine->Draw(line, 2, color);
	pLine->End();

	// Release the line
	pLine->Release();
}
const ImGuiViewport* viewport;
int main(int, char**)
{
	//20 - 150
	// Get the value of the WINDIR environment variable
	wchar_t windir[MAX_PATH];
	GetEnvironmentVariable(L"WINDIR", windir, MAX_PATH);

	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	std::string strWindir = converter.to_bytes(windir);

	std::string arialFontPath = strWindir + "\\Fonts\\arial.ttf";
	//std::string japFontPath = strWindir + "\\Fonts\\YuGothM.ttc";
//	std::string extFontPath = strWindir + "\\Fonts\\tahoma.ttf";

	//check if arial font exists
	if (std::filesystem::exists(strWindir)) {
		// all good
	}
	else {
		if (std::filesystem::exists("%WINDIR%/")) {
			MessageBox(NULL, L"Arial Font not found", L"Error", MB_OK | MB_TASKMODAL);
			return 0;
		}
		else {
			MessageBox(NULL, L"Couldn't get WIN-DIR path", L"Error", MB_OK | MB_TASKMODAL);
			return 0;
		}
	}
	struct CUSTOMVERTEX
	{
		D3DXVECTOR3 position;
		DWORD color;
	};

	// Create application window
	ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Test", nullptr };
	::RegisterClassExW(&wc);
	hwnd = ::CreateWindowW(wc.lpszClassName, L"BASS Test", WS_OVERLAPPEDWINDOW, 100, 100, 1600, 900, nullptr, nullptr, wc.hInstance, nullptr);
	ShowWindow(hwnd, SW_SHOWDEFAULT); //display window

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Setup Dear ImGui context
//	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.WantCaptureMouse = true;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);

	ImGui_ImplDX9_Init(g_pd3dDevice);

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.00f);

	// Main loop
	bool done = false;

	ImVector<ImWchar> ranges;
	ImFontGlyphRangesBuilder builder;
	builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
	builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
	builder.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
	builder.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
	builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
	builder.AddRanges(io.Fonts->GetGlyphRangesGreek());
	builder.AddRanges(io.Fonts->GetGlyphRangesKorean());
	builder.AddRanges(io.Fonts->GetGlyphRangesThai());
	builder.AddRanges(io.Fonts->GetGlyphRangesVietnamese());
	builder.BuildRanges(&ranges);
	ImFont* Arial = io.Fonts->AddFontFromFileTTF(arialFontPath.c_str(), 18, nullptr, ranges.Data);
	//ImFont* japFont = io.Fonts->AddFontFromFileTTF(japFontPath.c_str(), 18, nullptr, ranges.Data);
	//ImFont* extFont = io.Fonts->AddFontFromFileTTF(extFontPath.c_str(), 18, nullptr, ranges.Data);
	ImFont* ArialSmall = io.Fonts->AddFontFromFileTTF(arialFontPath.c_str(), 14, nullptr, ranges.Data);

	const D3DCOLOR black = D3DCOLOR_ARGB(255, 0, 0, 0);
	const D3DCOLOR white = D3DCOLOR_ARGB(255, 255, 255, 255);
	const D3DCOLOR red = D3DCOLOR_ARGB(255, 255, 0, 0);
	const D3DCOLOR red_hz = D3DCOLOR_ARGB(100, 255, 0, 0);
	const D3DCOLOR blue = D3DCOLOR_ARGB(255, 0, 0, 255);
	const D3DCOLOR green = D3DCOLOR_ARGB(255, 0, 255, 0);
	const D3DCOLOR gray = D3DCOLOR_ARGB(255, 56, 56, 56);
	const D3DCOLOR lightblue = D3DCOLOR_ARGB(255, 23, 220, 255);

#pragma region BASS

	int device = -1; // Default Sounddevice
	int freq = 48000; // Sample rate (Hz)
	HSTREAM streamHandle; // Handle for sample
	// init plugins
	const int pluginCount = 15;
	const char* pluginNames[pluginCount];
	pluginNames[0] = "basscd";
	pluginNames[1] = "bassflac";
	pluginNames[2] = "basshls";
	pluginNames[3] = "bassopus";
	pluginNames[4] = "basswasapi";
	pluginNames[5] = "basswma";
	pluginNames[6] = "basswv";
	pluginNames[7] = "bassalac";
	pluginNames[8] = "bassape";
	pluginNames[9] = "bassdsd";
	pluginNames[10] = "bassmidi";
	pluginNames[11] = "basswebm";
	pluginNames[12] = "bassenc";
	pluginNames[13] = "bassmix";
	pluginNames[14] = "bass_fx";
	BASS_Init(device, freq, 0, 0, NULL);
	HPLUGIN pluginArray[pluginCount];

	for (int i = 0; i < pluginCount; i++) {
		pluginArray[i] = BASS_PluginLoad(pluginNames[i], 0);
	}

	/* Initialize output device */

	float bass_volume = 0.12f;
	float bass_cpu = 0.0f;

	int bass_sec = 0;
	std::string filepath = "C:\\aaa\\Super Eurobeat Mix Vol.5 (3 HOUR SPECIAL) (192kbps).mp3";
	streamHandle = BASS_StreamCreateFile(false, filepath.c_str(), 0, 0, BASS_SAMPLE_FLOAT | BASS_STREAM_AUTOFREE | BASS_ASYNCFILE | BASS_STREAM_PRESCAN | BASS_SAMPLE_LOOP);
	BASS_ChannelSetAttribute(streamHandle, BASS_ATTRIB_VOL, bass_volume);
	BASS_ChannelPlay(streamHandle, TRUE);
	bass_cpu = BASS_GetCPU();
	float bitrate, sample_rate;

#pragma endregion
	//47
#pragma region ColorFormatToString
	const char* colorFormatName[84];
	colorFormatName[0] = "D3DFMT_UNKNOWN";
	colorFormatName[20] = "D3DFMT_R8G8B8";
	colorFormatName[21] = "D3DFMT_A8R8G8B8";
	colorFormatName[22] = "D3DFMT_X8R8G8B8";
	colorFormatName[23] = "D3DFMT_R5G6B5";
	colorFormatName[24] = "D3DFMT_X1R5G5B5";
	colorFormatName[25] = "D3DFMT_A1R5G5B5";
	colorFormatName[26] = "D3DFMT_A4R4G4B4";
	colorFormatName[27] = "D3DFMT_R3G3B2";
	colorFormatName[28] = "D3DFMT_A8";
	colorFormatName[29] = "D3DFMT_A8R3G3B2";
	colorFormatName[30] = "D3DFMT_X4R4G4B4";
	colorFormatName[31] = "D3DFMT_A2B10G10R10";
	colorFormatName[32] = "D3DFMT_A8B8G8R8";
	colorFormatName[33] = "D3DFMT_X8B8G8R8";
	colorFormatName[34] = "D3DFMT_G16R16";
	colorFormatName[35] = "D3DFMT_A2R10G10B10";
	colorFormatName[36] = "D3DFMT_A16B16G16R16";
	colorFormatName[40] = "D3DFMT_A8P8";
	colorFormatName[41] = "D3DFMT_P8";
	colorFormatName[50] = "D3DFMT_L8";
	colorFormatName[51] = "D3DFMT_A8L8";
	colorFormatName[52] = "D3DFMT_A4L4";
	colorFormatName[60] = "D3DFMT_V8U8";
	colorFormatName[61] = "D3DFMT_L6V5U5";
	colorFormatName[62] = "D3DFMT_X8L8V8U8";
	colorFormatName[63] = "D3DFMT_Q8W8V8U8";
	colorFormatName[64] = "D3DFMT_V16U16";
	colorFormatName[67] = "D3DFMT_A2W10V10U10";
	colorFormatName[70] = "D3DFMT_D16_LOCKABLE";
	colorFormatName[71] = "D3DFMT_D32";
	colorFormatName[73] = "D3DFMT_D15S1";
	colorFormatName[75] = "D3DFMT_D24S8";
	colorFormatName[77] = "D3DFMT_D24X8";
	colorFormatName[79] = "D3DFMT_D24X4S4";
	colorFormatName[80] = "D3DFMT_D16";
	colorFormatName[82] = "D3DFMT_D32F_LOCKABLE";
	colorFormatName[83] = "D3DFMT_D24FS8";
#pragma endregion
	//	LPD3DXLINE pLine;

	// Create the line object
	if (FAILED(D3DXCreateLine(g_pd3dDevice, &dxLine)))
	{
		//MessageBox(NULL, L"dxline creation failed", L"Error", MB_OK | MB_TASKMODAL);
	}
	// Create the line object
	if (FAILED(D3DXCreateLine(g_pd3dDevice, &dxLine2)))
	{
		//MessageBox(NULL, L"dxline creation failed", L"Error", MB_OK | MB_TASKMODAL);
	}

	// VARS:
	bool db_draw = true, hz_draw = true;
	bool db_aa = false, hz_aa = false;
	bool midLine = false;
	bool midLine_follow = false;
	bool boost_low = false;
	int db_amp = 120;
	int hz_amp = 2000;
	float lowBoost = 1.7f;
	int max_freq = 350;
	int sample_start = 0;
	// init with 8192 FFT Samples
	uintptr_t initFFT = 0x80000005;
	int initFFTdata = 8192;// = 4096 (4095)
	int numLinesDyn = (initFFTdata / 2) - 1;
	int midHZ;

	// view:
	bool pluginFrame = false;
	bool showDataArray = false;
	bool showDxInfo = false;
	while (!done)
	{
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		// Handle window resize (we don't resize directly in the WM_SIZE handler)
		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			g_d3dpp.BackBufferWidth = g_ResizeWidth;
			g_d3dpp.BackBufferHeight = g_ResizeHeight;
			g_ResizeWidth = g_ResizeHeight = 0;
			ResetDevice();
		}

		//  Start the Dear ImGui frame
		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX9_NewFrame();

		// init to max value
		const int STATIC_FFT_DATA = 32768;
		const int numLines = (STATIC_FFT_DATA / 2) - 1;

		int true_STATIC_FFT_DATA = initFFTdata;
		int true_numLines = (true_STATIC_FFT_DATA / 2) - 1;

		float fft[STATIC_FFT_DATA];
		BASS_ChannelGetData(streamHandle, fft, initFFT); // get the FFT data
		//	const int numLines = sizeof(fft) / sizeof(int) - 1;
		float db[numLines];
		for (int i = 1; i < true_numLines + 1; i++) {
			db[i - 1] = log10f((fft[i])) * 10;
		}

		ImGui::NewFrame();
		ImGui::PushFont(Arial);

		ImGui::BeginMainMenuBar();
		float mainMenuBarHeight = ImGui::GetFrameHeight();
		if (ImGui::BeginMenu("Main"))
		{
			ImGui::Text("Test");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			ImGui::Checkbox("Show FFT Data Array Content", &showDataArray);
			ImGui::Checkbox("Show Plugin Info", &pluginFrame);
			ImGui::Checkbox("Show DirectX Info", &showDxInfo);

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
		ImGui::Begin("Debug");

		viewport = ImGui::GetMainViewport();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::Text("X %.1f", io.MousePos.x);
		ImGui::Text("Y %.1f", io.MousePos.y);
		ImGui::Text("VX %.0f", viewport->WorkSize.x);
		ImGui::Text("VY %.0f", viewport->WorkSize.y);
		ImGui::Text("dB MAX: %.1f", std::max_element(db, db + sizeof(true_numLines) / sizeof(db[0])));
		ImGui::Text("dB min: %.1f", std::min_element(db, db + sizeof(true_numLines) / sizeof(db[0])));

		BASS_ChannelGetAttribute(streamHandle, BASS_ATTRIB_BITRATE, &bitrate);
		BASS_ChannelGetAttribute(streamHandle, BASS_ATTRIB_FREQ, &sample_rate);
		bass_cpu = BASS_GetCPU();
		ImGui::Text("BASS CPU: %.2f%%", bass_cpu);

		ImGui::Text("BitRate %.20f", bitrate);
		ImGui::Text("SampleRate %.0f", sample_rate);
		ImGui::Text(("Current Time: " + convertSecondsToTime(BASS_ChannelBytes2Seconds(streamHandle, BASS_ChannelGetPosition(streamHandle, BASS_POS_BYTE))) + " | " + std::to_string(BASS_ChannelBytes2Seconds(streamHandle, BASS_ChannelGetPosition(streamHandle, BASS_POS_BYTE)))).c_str());
		ImGui::Text(("Max Time: " + convertSecondsToTime(BASS_ChannelBytes2Seconds(streamHandle, BASS_ChannelGetLength(streamHandle, BASS_POS_BYTE))) + " | " + std::to_string(BASS_ChannelBytes2Seconds(streamHandle, BASS_ChannelGetLength(streamHandle, BASS_POS_BYTE)))).c_str());
		ImGui::Checkbox("Draw dB", &db_draw);
		ImGui::SameLine();
		ImGui::Checkbox("dB AA", &db_aa);
		ImGui::SliderInt("dB Amplifier", &db_amp, 1, 1000);
		ImGui::Checkbox("Draw Hz", &hz_draw);
		ImGui::SameLine();
		ImGui::Checkbox("Hz AA", &hz_aa);
		ImGui::SliderInt("Hz Amplifier", &hz_amp, 1, 50000);

		std::string sampleText = "Sample Cut Out | Range(" + std::to_string((int)(((sample_rate / 2) / (true_numLines)) * sample_start)) + "Hz - " + std::to_string((int)(((sample_rate / 2) / (true_numLines)) * numLinesDyn)) + ("Hz of ") + std::to_string((int)(sample_rate / 2)) + "Hz are displayed)";
		ImGui::Text(sampleText.c_str());
		midHZ = ((((sample_rate / 2) / (true_numLines)) * numLinesDyn) + (((sample_rate / 2) / (true_numLines)) * sample_start)) / 2;
		if (midLine_follow) {
			if (io.MousePos.x >= 0 && io.MousePos.x <= viewport->WorkSize.x) {
				midHZ = GetHzAtPos(sample_rate, true_numLines, numLinesDyn, viewport->WorkSize.x, io.MousePos.x);
				//midHZ = (((((sample_rate / 2) / (true_numLines)) * sample_start) / viewport->WorkSize.x) * io.MousePos.x) + (((sample_rate / 2) / (true_numLines)) * sample_start);
			}
			else {
				midHZ = ((((sample_rate / 2) / (true_numLines)) * numLinesDyn) + (((sample_rate / 2) / (true_numLines)) * sample_start) / 2);
			}
		}
		ImGui::Checkbox(("Draw Middle Hz Line (" + std::to_string(midHZ) + "Hz)").c_str(), &midLine);
		ImGui::SameLine();
		ImGui::Checkbox("Follow Cursor X (Broken when changing Sample Start)", &midLine_follow);
		ImGui::Checkbox(("Boost Low Frequencies (0Hz-" + std::to_string(max_freq) + "Hz)").c_str(), &boost_low);
		ImGui::SliderFloat("Low Frequency Boost Multiplier", &lowBoost, 1.0f, 4.0f);
		ImGui::SliderInt("Max Frequency", &max_freq, 20, 1000);
		ImGui::SliderInt("Sample Cut Out", &numLinesDyn, 0, true_numLines);
		ImGui::SliderInt("Sample Start", &sample_start, 0, numLinesDyn);

		if (sample_start > numLinesDyn) {
			sample_start = numLinesDyn;
		}

		ImGui::Text(("Set FFT Size (* a lot of samples, might cause low FPS), Current FFT: (0x%08X/" + std::to_string(initFFTdata) + ")").c_str(), DWORD(initFFT));
		if (ImGui::Button("256 FFT")) {
			initFFT = 0x80000000;
			initFFTdata = 256;
			//reset scaling
			numLinesDyn = (initFFTdata / 2) - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("512 FFT")) {
			initFFT = 0x80000001;
			initFFTdata = 512;
			//reset scaling
			numLinesDyn = (initFFTdata / 2) - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("1024 FFT")) {
			initFFT = 0x80000002;
			initFFTdata = 1024;
			//reset scaling
			numLinesDyn = (initFFTdata / 2) - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("2048 FFT")) {
			initFFT = 0x80000003;
			initFFTdata = 2048;
			//reset scaling
			numLinesDyn = (initFFTdata / 2) - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("4096 FFT")) {
			initFFT = 0x80000004;
			initFFTdata = 4096;
			//reset scaling
			numLinesDyn = (initFFTdata / 2) - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("8192 FFT")) {
			initFFT = 0x80000005;
			initFFTdata = 8192;
			//reset scaling
			numLinesDyn = (initFFTdata / 2) - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("16384 FFT*")) {
			initFFT = 0x80000006;
			initFFTdata = 16384;
			//reset scaling
			numLinesDyn = (initFFTdata / 2) - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("32768 FFT*")) {
			initFFT = 0x80000007;
			initFFTdata = 32768;
			//reset scaling
			numLinesDyn = (initFFTdata / 2) - 1;
		}
		ImGui::Text("--BASS--");
		if (ImGui::SliderFloat("Volume", &bass_volume, 0.0f, 2.0f)) {
			BASS_ChannelSetAttribute(streamHandle, BASS_ATTRIB_VOL, bass_volume);
		}
		if (ImGui::Button("<- 10s")) {
			BASS_ChannelSetPosition(streamHandle, BASS_ChannelSeconds2Bytes(streamHandle, BASS_ChannelBytes2Seconds(streamHandle, BASS_ChannelGetPosition(streamHandle, BASS_POS_BYTE)) - 10), BASS_POS_BYTE);
		}
		ImGui::SameLine();
		if (ImGui::Button("10s ->")) {
			BASS_ChannelSetPosition(streamHandle, BASS_ChannelSeconds2Bytes(streamHandle, BASS_ChannelBytes2Seconds(streamHandle, BASS_ChannelGetPosition(streamHandle, BASS_POS_BYTE)) + 10), BASS_POS_BYTE);
		}
		ImGui::SameLine();
		if (ImGui::Button("Resume")) {
			BASS_ChannelPlay(streamHandle, false);
		}
		ImGui::SameLine();
		if (ImGui::Button("Pause")) {
			BASS_ChannelPause(streamHandle);
		}
		ImGui::SameLine();

		if (ImGui::Button("Play From Path")) {
			BASS_StreamFree(streamHandle);
			streamHandle = BASS_StreamCreateFile(false, filepath.c_str(), 0, 0, BASS_SAMPLE_FLOAT | BASS_STREAM_AUTOFREE | BASS_ASYNCFILE | BASS_STREAM_PRESCAN | BASS_SAMPLE_LOOP);
			BASS_ChannelSetAttribute(streamHandle, BASS_ATTRIB_VOL, bass_volume);
			BASS_ChannelPlay(streamHandle, TRUE);
		}
		ImGui::InputText((const char*)u8"File Path", &filepath);

		int byte_pos = BASS_ChannelBytes2Seconds(streamHandle, BASS_ChannelGetPosition(streamHandle, BASS_POS_BYTE));
		if (ImGui::SliderInt("Fast Seek", &byte_pos, 0, BASS_ChannelBytes2Seconds(streamHandle, BASS_ChannelGetLength(streamHandle, BASS_POS_BYTE)))) {
			BASS_ChannelSetPosition(streamHandle, BASS_ChannelSeconds2Bytes(streamHandle, byte_pos), BASS_POS_BYTE);
		}
		ImGui::End();
		int colorStackPushed = 0;
		if (showDataArray) {
			ImGui::Begin(("Array View (" + std::to_string((initFFTdata / 2) - 1) + ")").c_str(), &showDataArray);
			ImGui::PushFont(ArialSmall);
			for (int i = 1; i <= (initFFTdata / 2) - 1; i++) {
				if ((int)(fft[i] * 1000) == 0) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 255, 255, 255));
					colorStackPushed++;
				}
				if ((int)(fft[i] * 1000) > 0) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 255, 0, 255));
					colorStackPushed++;
				}
				if ((int)(fft[i] * 1000) >= 10) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 0, 0, 255));
					colorStackPushed++;
				}
				if ((int)(fft[i] * 1000) >= 20) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 255, 255));
					colorStackPushed++;
				}
				ImGui::Text(("[" + std::to_string(i) + "] %.0f").c_str(), (fft[i] * 1000));
				ImGui::SameLine();
				ImGui::Text(" | %.20f", fft[i]);
			}
			ImGui::End();
		}
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 255, 255, 255));
		colorStackPushed++;
		if (pluginFrame) {
			ImGui::PushFont(Arial);
			ImGui::Begin("Plugin Info", &pluginFrame, ImGuiWindowFlags_AlwaysAutoResize);

			if (ImGui::CollapsingHeader(("Plugin Handles (" + std::to_string(pluginCount) + ")").c_str()))
			{
				for (int i = 0; i < pluginCount; i++) {
					if (pluginArray[i] == 0) {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 255, 255, 255));
						colorStackPushed++;
						ImGui::Text((pluginNames[i] + (std::string)(" = ") + std::to_string(pluginArray[i]) + " | Not Loaded").c_str());
					}
					else {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 255, 0, 255));
						colorStackPushed++;
						ImGui::Text((pluginNames[i] + (std::string)(" = ") + std::to_string(pluginArray[i]) + " | Loaded").c_str());
					}
				}
			}
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 255, 255, 255));
			colorStackPushed++;
			if (ImGui::CollapsingHeader("Plugin Info (only loaded plugins showed)"))
			{
				if (ImGui::CollapsingHeader("bass"))
				{
					ImGui::Text("Plugin Version: 0x%08X", BASS_GetVersion());
					ImGui::Text("Formats: MP3/MP2/MP1/OGG/WAV/AIFF");
				}

				for (int i = 0; i < pluginCount; i++) {
					if (pluginArray[i] != 0) {
						if (ImGui::CollapsingHeader(pluginNames[i]))
						{
							const BASS_PLUGININFO* info = BASS_PluginGetInfo(pluginArray[i]); // get the plugin info
							ImGui::Text("Plugin Version: 0x%08X", info->version);
							for (int a = 0; a < info->formatc; a++) { // display the array of formats...
								ImGui::Text(info->formats[a].name);
								ImGui::SameLine();
								ImGui::Text("=");
								ImGui::SameLine();
								ImGui::Text(info->formats[a].exts);
							}
						}
					}
				}
			}

			ImGui::End();
		}

		if (showDxInfo) {
			ImGui::PushFont(Arial);
			ImGui::Begin("DX Info", &showDxInfo, ImGuiWindowFlags_AlwaysAutoResize);

			D3DDISPLAYMODE mode;
			D3DADAPTER_IDENTIFIER9 identifier;
			g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);
			g_pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, NULL, &identifier);
			ImGui::Text("Display:");
			ImGui::SameLine();
			ImGui::Text(identifier.DeviceName);
			ImGui::Text("Color Format:");
			ImGui::SameLine();
			if (colorFormatName[D3DFORMAT(mode.Format)] == "") {
				ImGui::Text((std::to_string(D3DFORMAT(mode.Format))).c_str());
			}
			else {
				ImGui::Text(colorFormatName[D3DFORMAT(mode.Format)]);
			}
			ImGui::Text(("Screen: " + std::to_string(mode.Width) + "x" + std::to_string(mode.Height) + "@" + std::to_string(mode.RefreshRate) + "Hz").c_str());
			ImGui::Text("Description:");
			ImGui::SameLine();
			ImGui::Text(identifier.Description);
			ImGui::Text("Device ID:");
			ImGui::SameLine();
			ImGui::Text((std::to_string(identifier.DeviceId)).c_str());
			ImGui::Text("Vendor ID:");
			ImGui::SameLine();
			ImGui::Text((std::to_string(identifier.VendorId)).c_str());
			ImGui::Text("Revision:");
			ImGui::SameLine();
			ImGui::Text((std::to_string(identifier.Revision)).c_str());
			ImGui::Text("SubSys ID:");
			ImGui::SameLine();
			ImGui::Text((std::to_string(identifier.SubSysId)).c_str());
			ImGui::Text("WHQLLevel:");
			ImGui::SameLine();
			ImGui::Text((std::to_string(identifier.WHQLLevel)).c_str());
			ImGui::Text("Driver:");
			ImGui::SameLine();
			ImGui::Text(identifier.Driver);


			ImGui::End();
		}
		// Rendering
		ImGui::EndFrame();
		// Application WILL Memory Leak without this
		ImGui::PopStyleColor(colorStackPushed + 10000);
		colorStackPushed = 0;
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 20.0f), (int)(clear_color.y * clear_color.w * 20.0f), (int)(clear_color.z * clear_color.w * 20.0f), (int)(clear_color.w * 255.0f));
		g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 0.5f, 0);
		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();

			// Create an array to hold the start and end points of each line
			D3DXVECTOR2 dblines[numLines * 2];
			D3DXVECTOR2 hzlines[numLines * 2];
			int lowFcount = 0;
			for (int i = 1 + sample_start; i <= true_numLines; i++) {
				if (GetHzAtArrayPos(sample_rate, true_numLines, i) >= 0 && GetHzAtArrayPos(sample_rate, true_numLines, i) <= max_freq) {
					lowFcount++;
				}
			}
			//float lowBoost = 1.7f;
			float lowFAmpStepDown = lowBoost / lowFcount;
			int lowFF = 0;
			for (int i = 1; i <= numLinesDyn; i++) {
				//db
				if (db_draw) {
					if (boost_low) {
						if (GetHzAtArrayPos(sample_rate, true_numLines, i + sample_start) >= 0 && GetHzAtArrayPos(sample_rate, true_numLines, i + sample_start) <= max_freq) {
							dblines[i * 2] = D3DXVECTOR2(float((i)) / float((float(numLinesDyn - sample_start) / float((viewport->WorkSize.x * 1)))), ((viewport->WorkSize.y + mainMenuBarHeight) + (((float)db_amp * (float)((lowBoost - (lowFAmpStepDown * lowFF)) + 1)) / log10f((fft[(i + 1) + sample_start]))))); // Line end
							lowFF++;
						}
						else {
							dblines[i * 2] = D3DXVECTOR2(float((i)) / float((float(numLinesDyn - sample_start) / float((viewport->WorkSize.x * 1)))), ((viewport->WorkSize.y + mainMenuBarHeight) + (db_amp / log10f((fft[(i + 1) + sample_start]))))); // Line end
						}
					}
					else {
						dblines[i * 2] = D3DXVECTOR2(float((i)) / float((float(numLinesDyn - sample_start) / float((viewport->WorkSize.x * 1)))), ((viewport->WorkSize.y + mainMenuBarHeight) + (db_amp / log10f((fft[(i + 1) + sample_start]))))); // Line end
					}
					dblines[i * 2 + 1] = D3DXVECTOR2(float((i)) / float((float(numLinesDyn - sample_start) / float((viewport->WorkSize.x * 1)))), viewport->WorkSize.y + mainMenuBarHeight); // Line start
				}
				//magnitude
				if (hz_draw) {
					hzlines[i * 2] = D3DXVECTOR2(float((i)) / float((float(numLinesDyn - sample_start) / float((viewport->WorkSize.x * 1)))), ((viewport->WorkSize.y + mainMenuBarHeight) - (fft[(i + 1) + sample_start]) * hz_amp)); // Line end
					hzlines[i * 2 + 1] = D3DXVECTOR2(float((i)) / float((float(numLinesDyn - sample_start) / float((viewport->WorkSize.x * 1)))), viewport->WorkSize.y + mainMenuBarHeight); // Line start
				}
			}

			// DRAW
			dxLine->SetWidth(viewport->WorkSize.x / float(numLinesDyn - sample_start));
			dxLine->SetAntialias(db_aa);
			dxLine->Begin();
			for (int i = 0; i < numLinesDyn; i++)
			{
				if (db_draw) {
					if (boost_low) {
						if (GetHzAtArrayPos(sample_rate, true_numLines, i) >= 0 && GetHzAtArrayPos(sample_rate, true_numLines, i) <= max_freq) {
							dxLine->SetWidth((viewport->WorkSize.x / float(numLinesDyn)) * 5.0f);
							dxLine->Draw(&dblines[i * 2], 2, white);
						}
						else {
							dxLine->SetWidth(viewport->WorkSize.x / float(numLinesDyn));
							dxLine->Draw(&dblines[i * 2], 2, white);
						}
					}
					else {
						dxLine->SetWidth(viewport->WorkSize.x / float(numLinesDyn));
						dxLine->Draw(&dblines[i * 2], 2, white);
					}
				}
			}
			dxLine->End();
			dxLine2->SetWidth(viewport->WorkSize.x / float(numLinesDyn - sample_start));
			dxLine2->SetAntialias(hz_aa);
			dxLine2->Begin();
			for (int i = 0; i < numLinesDyn; i++)
			{
				if (hz_draw) {
					dxLine2->Draw(&hzlines[i * 2], 2, red_hz);
				}
			}
			dxLine2->End();
			if (midLine) {
				if (midLine_follow) {
					if (io.MousePos.x >= 0 && io.MousePos.x <= viewport->WorkSize.x) {
						DrawLine(g_pd3dDevice, io.MousePos.x, viewport->WorkSize.y, io.MousePos.x, 0, 3.0f, green);
					}
					else {
						DrawLine(g_pd3dDevice, viewport->WorkSize.x / 2, viewport->WorkSize.y, viewport->WorkSize.x / 2, 0, 3.0f, green);
					}
				}
				else {
					DrawLine(g_pd3dDevice, viewport->WorkSize.x / 2, viewport->WorkSize.y, viewport->WorkSize.x / 2, 0, 3.0f, green);
				}
			}

			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		}
		g_pd3dDevice->EndScene();

		HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

		// Handle loss of D3D9 device
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			ResetDevice();
	}

	BASS_Free();
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);
	/* Once you are done with your sample you should free it */
	// BASS_SampleFree(streamHandle);
	return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
		return false;

	// Create the D3DDevice
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync

	//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
	g_d3dpp.MultiSampleQuality = 0;
	g_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;

	return true;
}

void CleanupDeviceD3D()
{
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void ResetDevice()
{
	std::cout << "Device Reset Called" << std::endl;

	ImGui_ImplDX9_InvalidateDeviceObjects();

	// Line must be released after Device Objects got invalidated
	dxLine->Release();
	dxLine2->Release();

	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL) {
		IM_ASSERT(0);
	}

	ImGui_ImplDX9_CreateDeviceObjects();

	// Create the line object again due to it was released
	if (FAILED(D3DXCreateLine(g_pd3dDevice, &dxLine)))
	{
		std::cout << "dxLine creation failed" << std::endl;
	}
	if (FAILED(D3DXCreateLine(g_pd3dDevice, &dxLine2)))
	{
		std::cout << "dxLine creation failed" << std::endl;
	}
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{

	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;

		g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
		g_ResizeHeight = (UINT)HIWORD(lParam);


		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

// IMGUI.CPP | LINE 9380
// IMGUI.CPP | LINE 3090