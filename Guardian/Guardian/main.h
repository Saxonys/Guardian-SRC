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
        inline std::vector<sstool> javaw = {
            {"Generic Client", {
                "keystr0kes/UT",
                ",(IILnet/minecraft/entity/EntityLivingBase;)V",
                "3Ljava/lang/Class<Lnet/minecraft/client/Minecraft;>;",
                "I(Lnet/minecraftforge/event/entity/living/LivingEvent$LivingUpdateEvent;)V",
                "(DDF)[Ljava/lang/Object;!",
                "(Lkeystrokesmod/b9;)V!",
                "^0_=`HbTc\edf}g",
                "<tTgi",
                "i%'Jb",
                "'gkc9",
                "%(Ljava/net/URLStreamHandlerFactory;)V",
                "##Combo%02d",
                "TCNH$1",
                "<>X%fF%",
                "([SSl)Z",
                "i/xx$Op.classUT",
                "^0_=`HbTc\edf}g",
                "jdGyW`!z@",
                "i%'Jb",
                "%B7:+",
                "r8C*E",
                "_W_Y6",
                "mincpsEST.MF",
                "maxcpsEST.MF",
                "minreachEST.MF",
                "maxreachEST.MF",
                "net/minecraftxray/cv",
                "hypixel/xray/bypass/xray$1",
            }}
            ,{"Gucci Ghost Client", {
                "xyz/gucciclient/modules/Module",
                "Lxyz/gucciclient/utils/MovementUtils;",
                "xyz/gucciclient/UT",
                "xyz.gucciclient.modules.mods.utility.AgroPearl"
            }}
            ,{"Phantom Ghost Client", {
                "phantom\modules.properties",
                "PhantomClient.java"
            }}
            ,{"Cyanide Ghost Client", {
                "CyanideButtonUI.java"
            }}
            ,{"Drip Forge Ghost Client", {
                "<>{n@~O%",
                "<>AVU%A"
            }}
            ,{"Skilled Ghost Client", {
                "k(Ljava/lang/Class;)Lskilleddabful/"
            }}
            ,{"Schwifty Ghost Client", {
                "Invintory Empty, Disabling Finnsters Module"
            }}
            ,{"Incognito Ghost Client", {
                "<->bG1hb2hheA=="
            }}
            ,{"Kurium Ghost Client", {
                "dg82fo.pw"
            }}
            ,{"Liquidbounce Blatant Client", {
                "$LiquidBounce.kt",
                "/net/ccbluex/liquidbounc"
            }}
            ,{"Fenix Client", {
                "OModulePanic.java",
                "net/bplaced/azoq/module/modules/bedwars/ModuleEagle"
            }}
            ,{"Avix Ghost Client", {
                "me/aristhena/client/util/Timer",
                "me/aristhena/client/event/FlexibleArray$1"
            }}
            ,{"Drek Ghost Client", {
                "#8(19",
                "$A&*d"
            }}
            ,{"Trump Ghost Client", {
                "<>ZRX44",
                "=V;vUo"
            }}
            ,{"Lowkey Ghost Client", {
                "[Hamdulilha.",
                "Hamdulilha/",
            }}
            ,{"Universal Ghost Client", {
                "che4tlogprivcli3nt"
            }}
            ,{"Vea Ghost Client", {
                "L0t1,b",
                "R<f(9f",
                "net/kohi/tcpnodelaymod/coM2",
                "H<a-!b",
                "aSBsaWtlIGNvY2tz"
            }}
            ,{"Private Ghost Client", {
                "_/this.classPK"
            }}
            ,{"Syphlex Ghost Client", {
                "net/minecraft/scooby/util/ModeUtils.class"
            }}
            ,{"Vape V2 Ghost Client", {
                "KRTal",
                "yCcADi",
                ",#I)!",
                "C()[Lf/r;",
            }}
            ,{"Vape V3 Ghost Client", {
                "M(LiReSqtkUVgFIuiReSqtkUVg",
                "LiReSqtkUVgFWjiReSqtkUVg",
                "qHtZcIFG",
                "cskwAUfz"
            }}
            ,{"Vape V4 Ghost Client", {
                "->d=?ad=",
                "vapelogo"
            }}
            ,{"Demon Ghost Client", {
                "{~sG*u"
            }}
            , { "Zuiy Ghost Client", {
                "0SO1Lk2KASxzsd"
            } }
            , { "Bit Ghost Client", {
                "sl/steeldood/bit/client/module/impl/combat/ModuleAutoclicker"
            } }
            , { "Onetap Ghost Client", {
                "_/iffinal.class",
                "_/superenum.class"
            } }
            , { "Majenta Ghost Client", {
                "41/41/41/8/41"
            } }
            , { "Nero Ghost Client", {
                "u00e9",
                "u00e0"
            } }
            , { "Raven B1/B2 Ghost Client", {
                "keystrokesmod/bb$category!",
                "keystrokesmod/ax!"
                "(Lkeystrokesmod/b9;)V!"
            } }
            , { "Karma Cane Mod", {
                "CwAuRgUMCz8LGgQDLkYNCxEzHBFLIDQcAREcYA4dCwYFWV9RXWhaNwRfciwuTCk0DRxKCDMGDQYXOw4cShAuAQRKKDUeAQsCFQoCAAYuOAcWDC4BBwteYEhI",
                "me/xpenguinx/karma/mod/Mods/Cane.classUT"
            } }
            , { "Legend's Cane Mod", {
                "me.legend.gamma.Commands.cmdsCaneHitter.caneHitterCommandHandler"
            } }
            , { "Sigma Blatant Client", {
                "*.sigmaclient.info",
                "sigmaclient.info"
            } }
            , { "BIKERWare Ghost Client", {
                "Distance Threshold for targeting"
            } }
        };
        inline std::vector<sstool> explorer = {
            {"Generic Client", {
                "killaura.class",
                "killaura.java"
            }}
        };
        inline std::vector<sstool> pcasvc = {
            {"Generic Client", {
                "killaura.class",
                "killaura.java"
            }}
        };
        inline std::vector<sstool> lsass = {
            {"Generic Client", {
                "killaura.class",
                "killaura.java"
            }}
        };
        inline std::vector<sstool> dps = {
            {"Generic Client", {
                "killaura.class",
                "killaura.java"
            }}
        };
        inline std::vector<sstool> dwm = {
            {"Generic Client", {
                "killaura.class",
                "killaura.java"
            }}
        };
        inline std::vector<sstool> smartscreen = {
            {"Generic Client", {
                "killaura.class",
                "killaura.java"
            }}
        };
        inline std::vector<sstool> dgt = {
            {"Generic Client", {
                "killaura.class",
                "killaura.java"
            }}
        };
        inline std::vector<sstool> dnscache = {
            {"Generic Client", {
                "killaura.class",
                "killaura.java"
            }}
        };
    }
}