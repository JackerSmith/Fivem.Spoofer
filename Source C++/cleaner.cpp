Improve it and send me the corrected version.#include <iostream>
#include <Windows.h>
#include <string>
#include "basics.h"
#include "Spoofer.h"
#include "encryption.h"
#include "WEB.h"
#include "ImGui.h"

void DrawColoredSeparator(const std::string& id, const ImVec4& color) {
  static int separatorCount = 0;
  std::string separatorId = id + std::to_string(separatorCount++);

  ImGui::PushID(separatorId.c_str());

  ImGuiStyle& style = ImGui::GetStyle();
  ImVec2 windowPadding = style.WindowPadding;

  ImVec2 cursorPos = ImGui::GetCursorPos();
  cursorPos.x += windowPadding.x;

  ImVec2 regionSize = ImGui::GetContentRegionAvail();
  regionSize.x -= (2.0f * windowPadding.x);
  
  ImGui::PushStyleColor(ImGuiCol_Separator, color);

  ImGui::SetCursorPos(cursorPos);
  ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

  ImGui::PopStyleColor();

  cursorPos = ImGui::GetCursorPos();
  cursorPos.x += windowPadding.x;
  cursorPos.y += windowPadding.y + style.ItemSpacing.y;

  ImGui::SetCursorPos(cursorPos);

  ImGui::PopID();
}


DWORD FindProcessId(const std::wstring& processName)
{
    DWORD processId = 0;
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hProcessSnap, &pe32))
    {
        do
        {
            if (_tcsicmp(pe32.szExeFile, processName.c_str()) == 0)
            {
                processId = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);
    return processId;
}

void LogMessage(const std::string& message, int logType)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole != INVALID_HANDLE_VALUE)
    {
        SYSTEMTIME st, lt;
        GetLocalTime(&lt);

        std::cout << "[" << std::setfill('0') << std::setw(2) << lt.wHour << ":"
                  << std::setfill('0') << std::setw(2) << lt.wMinute << ":"
                  << std::setfill('0') << std::setw(2) << lt.wSecond << "] ";

        SetConsoleTextAttribute(hConsole, logType);
        std::cout << message << std::endl;

        SetConsoleTextAttribute(hConsole, 7);
    }
}


BOOL CALLBACK findWindowByTitle(HWND hwnd, LPARAM lParam)
{
    constexpr int MAX_TITLE_LENGTH = 1024;

    std::wstring window_title(MAX_TITLE_LENGTH, L'\0'); // create a string with MAX_TITLE_LENGTH null characters
    const int title_length = GetWindowTextW(hwnd, &window_title[0], MAX_TITLE_LENGTH);

    if (title_length == 0)
    {
        // Error getting window title, continue enumeration
        return TRUE;
    }

    window_title.resize(title_length); // trim the null characters from the end of the string

    const std::wstring& search_title = *reinterpret_cast<const std::wstring*>(lParam);
    if (window_title == search_title)
    {
        // Window title matches, save handle and stop enumeration
        valorant_window = hwnd;
        return FALSE;
    }

    // Window title does not match, continue enumeration
    return TRUE;
}



DWORD getProcessIdByName(const std::wstring& processName)
{
    DWORD processId = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32W process;
        ZeroMemory(&process, sizeof(process));
        process.dwSize = sizeof(process);

        if (Process32FirstW(snapshot, &process))
        {
            do
            {
                if (processName == process.szExeFile)
                {
                    processId = process.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snapshot, &process));
        }

        CloseHandle(snapshot);
    }
    return processId;
}

BOOL CALLBACK findWindowByTitle(HWND hwnd, LPARAM lParam)
{
    wchar_t windowTitle[256];
    GetWindowTextW(hwnd, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));

    if (wcscmp(windowTitle, (wchar_t*)lParam) == 0)
    {
        valorant_window = hwnd;
        return FALSE;
    }

    return TRUE;
}

// Declare global variables
DWORD g_pid = 0;
HWND g_hwnd = nullptr;

// Define callback function for EnumWindows
BOOL CALLBACK findWindowByTitle(HWND hwnd, LPARAM lParam)
{
    wchar_t buffer[1024];
    GetWindowTextW(hwnd, buffer, sizeof(buffer));
    if (std::wstring(buffer) == std::wstring(reinterpret_cast<const wchar_t*>(lParam)))
    {
        g_hwnd = hwnd;
        return FALSE;
    }
    return TRUE;
}

// Function to get the process ID by name
DWORD getProcessIdByName(const std::wstring& processName)
{
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Failed to create snapshot: " << GetLastError() << std::endl;
        return 0;
    }

    DWORD pid = 0;
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnap, &pe))
    {
        do
        {
            if (std::wstring(pe.szExeFile) == processName)
            {
                pid = pe.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnap, &pe));
    }

    CloseHandle(hSnap);

    if (pid == 0)
    {
        std::cerr << "Failed to find process: " << processName << std::endl;
    }

    return pid;
}
// Function to find the process ID given its name
DWORD getProcessIdByName(const std::wstring& processName)
{
    DWORD processId = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(snapshot, &processEntry))
        {
            do
            {
                if (_wcsicmp(processEntry.szExeFile, processName.c_str()) == 0)
                {
                    processId = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &processEntry));
        }
        CloseHandle(snapshot);
    }
    return processId;
}

BOOL CALLBACK findWindowByTitle(HWND hwnd, LPARAM lParam)
{
    constexpr int kBufferSize = 1024;
    wchar_t buffer[kBufferSize];

    // Check if the window is visible and has a title
    if (IsWindowVisible(hwnd) && GetWindowTextLengthW(hwnd) > 0)
    {
        // Get the window title text
        if (GetWindowTextW(hwnd, buffer, kBufferSize) > 0)
        {
            // Compare the title text with the string passed in as lParam
            if (wcscmp(buffer, reinterpret_cast<const wchar_t*>(lParam)) == 0)
            {
                // Match found, set the global variable and stop the enumeration
                g_hwnd = hwnd;
                return FALSE;
            }
        }
    }

    // Continue the enumeration
    return TRUE;
}

int main()
{
    const wchar_t* processName = L"val.exe";
    DWORD processId = 0;
    HWND hwnd = nullptr;

    // Find process ID and window handle
    {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 processEntry = { sizeof(PROCESSENTRY32) };
        if (Process32First(snapshot, &processEntry))
        {
            do {
                if (_wcsicmp(processEntry.szExeFile, processName) == 0)
                {
                    processId = processEntry.th32ProcessID;
                    hwnd = FindWindowW(nullptr, L"Valorant");
                    break;
                }
            } while (Process32Next(snapshot, &processEntry));
        }
        CloseHandle(snapshot);
    }

    if (processId == 0)
    {
        std::cerr << "Error: could not find process \"" << processName << "\"" << std::endl;
        return 1;
    }

    if (hwnd == nullptr)
    {
        std::cerr << "Error: could not find window \"Valorant\"" << std::endl;
        return 1;
    }

    // Get module base address
    HMODULE hModule = nullptr;
    DWORD cbNeeded = 0;
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess == nullptr || !EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded))
    {
        std::cerr << "Error: could not get module information" << std::endl;
        CloseHandle(hProcess);
        return 1;
    }

    MODULEINFO moduleInfo = { 0 };
    GetModuleInformation(hProcess, hModule, &moduleInfo, sizeof(moduleInfo));
    std::cout << "Process ID: " << processId << std::endl;
    std::cout << "Window handle: 0x" << hwnd << std::endl;
    std::cout << "Base address: 0x" << moduleInfo.lpBaseOfDll << std::endl;

    CloseHandle(hProcess);
    return 0;
}

 
