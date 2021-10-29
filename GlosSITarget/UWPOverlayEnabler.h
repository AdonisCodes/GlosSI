#pragma once

#include <filesystem>
#include <string>

#include "DllInjector.h"
#include "Overlay.h"

namespace UWPOverlayEnabler {

namespace internal {

inline std::filesystem::path EnablerPath()
{
    wchar_t buff[MAX_PATH];
    GetModuleFileName(GetModuleHandle(NULL), buff, MAX_PATH);
    const std::wstring path(buff);
    return path.substr(0, 1 + path.find_last_of(L'\\')) + L"UWPOverlayEnablerDLL.dll";
}

inline DWORD ExplorerPid()
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (Process32First(snapshot, &entry) == TRUE) {
        while (Process32Next(snapshot, &entry) == TRUE) {
            if (std::wstring(entry.szExeFile).find(L"explorer.exe") != std::string::npos) {
                return entry.th32ProcessID;
            }
        }
    }
    CloseHandle(snapshot);
    return 0;
}

}

inline void AddUwpOverlayOvWidget()
{
    Overlay::AddOverlayElem([]() {
        ImGui::SetNextWindowPos({950, 100}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints({170, 225}, {1000, 1000});
        ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
        ImGui::Begin("UWP-Overlay");
        ImGui::Text("To enable the overlay on top of \"fullscreen\" UWP-Apps,");
        ImGui::Text("a .dll has to be injected into explorer.exe");
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Text("This method also uses undocumented windows functions");
        ImGui::Text("and is highly experimental");
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Text("This might cause issues!!!");
        ImGui::Spacing();
        if (ImGui::CollapsingHeader("I am sure!")) {
            if (ImGui::Button(/* just */ "DO IT!")) { // insert meme gif here >.<
                const auto enabler_path = internal::EnablerPath();
                if (std::filesystem::exists(enabler_path)) {
                    const auto explorer_pid = internal::ExplorerPid();
                    if (explorer_pid != 0) {
                        if (DllInjector::TakeDebugPrivilege()) {
                            // No need to eject, as the dll is self-ejecting.
                            if (DllInjector::Inject(explorer_pid, enabler_path.wstring())) {
                                spdlog::info("Successfully injected UWPOverlay enabler into explorer.exe");
                                // Nesting level over 9000
                            }
                        }
                    }
                    else {
                        spdlog::error("explorer not found"); // needs loglevel WTF
                    }
                }
                else {
                    spdlog::error("UWPOverlayEnablerDLL not found");
                }
            }   
        }
        ImGui::End();
    });
}

}