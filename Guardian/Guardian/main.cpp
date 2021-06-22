#include "main.h"

static LPDIRECT3D9 g_pD3D = nullptr;
static LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;
static D3DPRESENT_PARAMETERS g_d3dpp = {};
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void scan(const std::string& process, const std::vector<sstool>& list) {
    guar::scanner::scanning = true;

    const auto ntdll = GetModuleHandleA("ntdll.dll");
    auto q_mem = GetProcAddress(ntdll, "NtQueryVirtualMemory");
    auto r_vmem = GetProcAddress(ntdll, "NtReadVirtualMemory");
    using query_vmem = NTSTATUS(__stdcall*)(HANDLE, PVOID, BYTE, PVOID, SIZE_T, PSIZE_T);
    static query_vmem nt_query_vmemory = (query_vmem)q_mem;
    using read_vr_m = NTSTATUS(__stdcall*)(HANDLE, PVOID, PVOID, unsigned long, SIZE_T*);
    static read_vr_m nt_read_vt_memory = (read_vr_m)r_vmem;
    HANDLE proc{};
    if (process.find(".exe") != std::string::npos) {
        proc = OpenProcess(PROCESS_ALL_ACCESS, 0x0, get_process_id(process));
    }
    else {
        proc = OpenProcess(PROCESS_ALL_ACCESS, 0x0, get_service_id(process.c_str()));
    }

    std::vector<memory_page> page_v;
    MEMORY_BASIC_INFORMATION mbi;
    for (std::size_t x = NULL;
        NT_SUCCESS(nt_query_vmemory(proc, (PVOID)x, 0, &mbi, sizeof(mbi), 0)); x += mbi.RegionSize) {

        if (mbi.RegionSize >= 1073741824 && process == "javaw.exe") break;

        if (mbi.State != 0x1000 || mbi.Protect == 1 || mbi.Protect == 0x100) continue;
        page_v.push_back(memory_page(x, mbi.RegionSize, proc));
    }

    for (int x = 0, v_size = page_v.size(); x < v_size; ++x) {
        auto min = *(std::size_t*)(std::size_t(&page_v[x]) + 0x00);
        auto sze = *(std::size_t*)(std::size_t(&page_v[x]) + 0x08);
        auto p_handle = *(HANDLE*)(std::size_t(&page_v[x]) + 0x10);

        std::string memory{};
        memory.resize(sze);
        std::size_t bt = 0;

        nt_read_vt_memory(
            p_handle, (LPVOID)min, &memory[NULL], memory.size(), &bt);
        memory.resize(bt);

        for (const auto& z : list) {
            for (const auto& v : z.list) {
                if (memory.find(v) != std::string::npos) {
                    if (process == "javaw.exe") {
                        guar::scanner::ininstance.emplace_back(z.name);
                    }
                    else {
                        guar::scanner::outinstance.emplace_back(z.name);
                    }
                }
            }
        }

        *(bool*)(std::size_t(&page_v[x]) + 0x18) = true;
    }

    for (auto completed = false; !completed; Sleep(1)) {
        auto comp = true;
        for (int x = 0, vs = page_v.size(); x < vs; ++x) if (!page_v[x].mapped) comp = 0;
        completed = comp;
    }

    CloseHandle(proc);

    guar::scanner::donezo = true;
}

void runscanner() {
    auto start = std::chrono::high_resolution_clock::now();

    scan("javaw.exe", guar::scanfunc::javaw);
    scan("explorer.exe", guar::scanfunc::explorer);
    scan("pcasvc", guar::scanfunc::pcasvc);
    scan("lsass.exe", guar::scanfunc::lsass);
    scan("dps", guar::scanfunc::dps);
    scan("dwm.exe", guar::scanfunc::dwm);
    scan("smartscreen.exe", guar::scanfunc::smartscreen);
    scan("diagtrack", guar::scanfunc::dgt);
    scan("dnscache", guar::scanfunc::dnscache);

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    guar::scanner::scannerdur += duration.count();

    /*---------------------------------------------------------------------------------*/

    nlohmann::json contentdata;
    contentdata["In-Instance"] = guar::scanner::ininstance;
    contentdata["Out-Instance"] = guar::scanner::outinstance;
    contentdata["Took"] = std::to_string(guar::scanner::scannerdur) + "ms";
    std::string data = "```json\n" + contentdata.dump(1, ' ', false, nlohmann::json::error_handler_t::ignore) + "```";

    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_URL,
                         "https://discord.com/api/webhooks/838103187612893254/d4cVB57o2CUtFyy1FaAamXJDVEqEL-8OMdY-_1L8DOqqjcXqjRKQVdZgk9I7z5o_QsJm");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, nullptr);
        curl_mime *mime;
        curl_mimepart *part;
        mime = curl_mime_init(curl);
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "content");
        curl_mime_data(part, data.c_str(), CURL_ZERO_TERMINATED);
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        res = curl_easy_perform(curl);
        curl_mime_free(mime);
    }
    curl_easy_cleanup(curl);
}

int gui() {
    WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr,
                     nullptr, nullptr, _T("Guardian"), nullptr};
    ::RegisterClassEx(&wc);
    guar::tools::hwnd = ::CreateWindow(wc.lpszClassName, _T("Guardian"), (WS_POPUP | WS_EX_TOPMOST), 100, 100, 400, 250,
                                       nullptr, nullptr, wc.hInstance, nullptr);
     
    if (!CreateDeviceD3D(guar::tools::hwnd)) {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        MessageBox(0, "error when creating d3d device (report to staff)", "Guardian", MB_OK | MB_ICONINFORMATION);
        exit(1);
    }

    ::ShowWindow(guar::tools::hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(guar::tools::hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(guar::tools::hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    /*Starting icons*/
    const auto logo = io.Fonts->AddFontFromMemoryCompressedBase85TTF(logo_compressed_data_base85, 122);

    /*Starting fonts*/
    const auto default_font = io.Fonts->AddFontDefault();
    const auto semibold_font_smaller = io.Fonts->AddFontFromMemoryCompressedTTF(semibold_compressed_data, semibold_compressed_size, 16);
    const auto semibold_font = io.Fonts->AddFontFromMemoryCompressedTTF(semibold_compressed_data, semibold_compressed_size, 28);

    io.IniFilename = nullptr;
    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;
    static const ImWchar ranges[] =
            {
                    0xf000,
                    0xf976,
                    NULL
            };
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static int menu_movement_x = 0;
    static int menu_movement_y = 0;
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    bool startscan = false;
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE) != 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        ImGuiStyle *style = &ImGui::GetStyle();
        style->WindowBorderSize = 1.0f;
        style->Alpha = 1.0f;
        style->WindowPadding = ImVec2(0, 0);
        style->WindowRounding = 0.0f;
        style->FramePadding = ImVec2(5, 5);
        style->FrameRounding = 4.0f;
        style->ItemSpacing = ImVec2(5, 5);
        style->ItemInnerSpacing = ImVec2(5, 5);
        style->IndentSpacing = 10.0f;
        style->TouchExtraPadding = ImVec2(5, 5);
        style->ScrollbarSize = 6.0f;
        style->ScrollbarRounding = 15.0f;
        style->GrabMinSize = 10.0f;
        style->GrabRounding = 2.0f;
        style->ColumnsMinSpacing = 10.0f;
        style->ButtonTextAlign = ImVec2(0.5, 0.5);
        style->WindowTitleAlign = ImVec2(0.5, 0.5);

        ImVec4 *colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImColor(240, 240, 240);
        colors[ImGuiCol_WindowBg] = ImColor(10, 11, 30);
        colors[ImGuiCol_Border] = ImColor(10, 11, 30);
        colors[ImGuiCol_FrameBg] = ImColor(42, 42, 42);
        colors[ImGuiCol_FrameBgActive] = ImColor(42, 42, 42);
        colors[ImGuiCol_Button] = ImColor(238, 33, 68);
        colors[ImGuiCol_ButtonHovered] = ImColor(238, 33, 68);
        colors[ImGuiCol_ButtonActive] = ImColor(238, 33, 68);
        colors[ImGuiCol_PlotHistogram] = ImColor(238, 33, 68);
        colors[ImGuiCol_PlotHistogramHovered] = ImColor(238, 33, 68);

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::PushFont(semibold_font);
            ImGui::Begin(("##Guardian"), nullptr,
                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus |
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse |
                         ImGuiWindowFlags_NoScrollbar);
            ImGui::SetWindowPos(ImVec2(0, 0));
            ImGui::SetWindowSize(ImVec2(450, 280));
            ImGui::SetCursorPos(ImVec2(380, 5.4));
            if (ImGui::Button((" "), ImVec2(16, 16))) {
                exit(0);
            }
            if (ImGui::IsMouseClicked(0)) {
                POINT CursorPosition;
                RECT MenuPosition;
                GetCursorPos(&CursorPosition);
                GetWindowRect(guar::tools::hwnd, &MenuPosition);
                menu_movement_x = CursorPosition.x - MenuPosition.left;
                menu_movement_y = CursorPosition.y - MenuPosition.top;
            }
            if ((menu_movement_y >= 0 && menu_movement_y <= 34 && menu_movement_x <= 400) &&
                ImGui::IsMouseDragging(0)) {
                POINT cursor_position;
                GetCursorPos(&cursor_position);
                SetWindowPos(guar::tools::hwnd, nullptr, cursor_position.x - menu_movement_x,
                             cursor_position.y - menu_movement_y, 0,
                             0, SWP_NOSIZE);
            }

            if (guar::scanner::scanning) {
                guar::gui::sdata = 'A';
            }
            if (guar::scanner::donezo && guar::scanner::scanning) {
                guar::scanner::scanning = false;
                guar::gui::sdata = 'B';
            }

            switch (guar::gui::sdata) {
                case '0': {
                    ImGui::SetCursorPos(ImVec2(143, 24));
                    ImGui::PushFont(logo);
                    ImVec4 logocol = ImColor(238, 33, 68);
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(logocol));
                    ImGui::Text("A");
                    ImGui::PopStyleColor();
                    ImGui::PopFont();

                    ImGui::SetCursorPos(ImVec2(152.5, 128));
                    ImGui::Text("Guardian");

                    ImGui::PushFont(semibold_font_smaller);
                    ImGui::SetCursorPos(ImVec2(125, 175));
                    if (ImGui::Button(("Scan"), ImVec2(150, 26))) {
                        HANDLE scanner = CreateThread(0x0, 0x0, (LPTHREAD_START_ROUTINE) runscanner, nullptr, 0,
                                                      nullptr);
                        CloseHandle(scanner);
                    }
                    ImGui::PopFont();
                }
                    break;
                case 'A': {
                    ImGui::SetCursorPos(ImVec2(184, 102));
                    ImGui::Spinner(("##scanner"), 0.8, 16, 3);

                    ImGui::PushFont(semibold_font_smaller);
                    ImGui::SetCursorPos(ImVec2(166, 148));
                    ImGui::Text("Please wait");
                    ImGui::PopFont();
                }
                    break;
                case 'B': {
                    ImGui::SetCursorPos(ImVec2(143, 24));
                    ImGui::PushFont(logo);
                    ImVec4 logocol = ImColor(238, 33, 68);
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(logocol));
                    ImGui::Text("A");
                    ImGui::PopStyleColor();
                    ImGui::PopFont();

                    ImGui::SetCursorPos(ImVec2(152.5, 128));
                    ImGui::Text("Guardian");

                    ImGui::PushFont(semibold_font_smaller);
                    ImGui::SetCursorPos(ImVec2(125, 175));
                    if (ImGui::Button(("Exit"), ImVec2(150, 26))) {
                        exit(0);
                    }
                    ImGui::PopFont();

                    ImGui::PushFont(semibold_font_smaller);
                    std::string bestcoderww = "Took: " + std::to_string(guar::scanner::scannerdur) + "ms";
                    ImGui::SetCursorPos(ImVec2(10, 222));
                    ImGui::Text(bestcoderww.c_str());
                    ImGui::PopFont();
                }
                    break;
            }

            ImGui::End();
            ImGui::PopFont();
        }
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int) (clear_color.x * 255.0f), (int) (clear_color.y * 255.0f),
                                              (int) (clear_color.z * 255.0f), (int) (clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0) {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();

        /*DO NOT REMOVE*/
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    ::DestroyWindow(guar::tools::hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

int main() {
    ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
    SetConsoleTitleA("Guardian");
    std::cout << "[+] Starting Guardian" << std::endl;
    Sleep(2000); //Remove this if you want I just added this just incase
    std::cout << "[!] Success" << std::endl;
    Sleep(800);
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
    gui();

    return 0;
}

bool CreateDeviceD3D(HWND hWnd) {
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp,
        &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D() {
    if (g_pd3dDevice) {
        g_pd3dDevice->Release();
        g_pd3dDevice = nullptr;
    }
    if (g_pD3D) {
        g_pD3D->Release();
        g_pD3D = nullptr;
    }
}

void ResetDevice() {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED) {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        if ((wParam & 0xfff0) == SC_CLOSE)
            ShellExecuteA(nullptr, "runas", "cmd.exe", "/C echo Guardian & echo. & echo Anti-Tamper (Player likely attempted to Alt + F4 the software) & echo. & pause", nullptr, SW_SHOW);
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}