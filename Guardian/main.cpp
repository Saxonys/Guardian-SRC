#include "main.h"
#include <Windows.h>
#include <string>

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

    std::string specialcheck1 = "7aB@1t3D"

        ReadProcessMemory(phandle, (char*)(Base_address + offset3), check1.c_str(), check1.length(), 0)
        WriteProcessMemory(phandle, (void*)(Base_address + offset3), check1.c_str(), check1.length(), 0)

    guar::scanner::donezo = true;
}

void runscanner() {
    auto start = std::chrono::high_resolution_clock::now();

    scan("yourprocesshere.exe", guar::scanfunc::yourprocesshere);

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
                         "Webhook to upload results to goes here");
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

// Gui code was removed but left the imgui files in the source so you can still code one if you'd like

int main() {
    ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
    SetConsoleTitleA("Guardian");
    std::cout << "[+] Starting Guardian" << std::endl;
    Sleep(2000); // Just here to improve the code
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

// Alt + F4 Software was removed for skidding reasons, code your own