#pragma once

#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <vector>
#include <string>
#include <d3d9.h>
#include <tchar.h>
#include <chrono>
#include <thread>

#include "misc/ext_l.hpp"
#include "misc/ntdll.hpp"
#include "misc/json.hpp"
#include "lib/curl/include/curl/curl.h"
#pragma comment(lib, "lib/curl/lib/libcurl_a.lib")
#pragma comment (lib, "Normaliz.lib")
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Wldap32.lib")
#pragma comment (lib, "Crypt32.lib")
#pragma comment (lib, "advapi32.lib")
#define DIRECTINPUT_VERSION 0x0800
#pragma comment(lib, "d3d9.lib")
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_internal.h"
#include "imgui/font.h"

inline DWORD get_process_id(const std::string &process_name) {
    PROCESSENTRY32 processInfo;
    processInfo.dwSize = sizeof(processInfo);
    HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (processesSnapshot == INVALID_HANDLE_VALUE)
        return 0;

    Process32First(processesSnapshot, &processInfo);
    if (!process_name.compare(processInfo.szExeFile)) {
        CloseHandle(processesSnapshot);

        return processInfo.th32ProcessID;
    }

    while (Process32Next(processesSnapshot, &processInfo)) {
        if (!process_name.compare(processInfo.szExeFile)) {
            CloseHandle(processesSnapshot);

            return processInfo.th32ProcessID;
        }
    }
    CloseHandle(processesSnapshot);

    return 0;
}

inline DWORD get_service_id(const char *service) {
    const auto h_scm = OpenSCManager(nullptr, nullptr, NULL);
    const auto h_sc = OpenService(h_scm, service, SERVICE_QUERY_STATUS);
    SERVICE_STATUS_PROCESS ssp = {};
    DWORD bytes_needed = 0;
    QueryServiceStatusEx(h_sc, SC_STATUS_PROCESS_INFO, reinterpret_cast<LPBYTE>(&ssp), sizeof(ssp), &bytes_needed);
    CloseServiceHandle(h_sc);
    CloseServiceHandle(h_scm);
    return ssp.dwProcessId;
}

inline size_t writeback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

struct sstool {
    std::string name = "";
    std::vector <std::string> list{};
};

namespace guar {
    namespace gui {
        inline char sdata = '0';
    }
    namespace tools {
        inline HWND hwnd{};
    }
    namespace scanner {
        inline bool scanning = false;
        inline bool donezo = false;
        inline int scannerdur = 0;
        inline std::string *read = new std::string;
        inline std::vector <std::string> ininstance;
        inline std::vector <std::string> outinstance;
    }
    namespace scanfunc {
        inline std::vector<sstool> yourprocesshere = {
            {"Your Client Name Here", {
                "Your Strings Here",
                "Your Strings Here" // Don't add a comma at the last string
            }}
        };
