#pragma once


#include <windows.h>
#include <string>
#include <iostream>

class AppState {
public:
    AppState(const std::wstring& fileName) : m_fileName(fileName) {}

    void SaveWindowState(HWND hwnd) {
        RECT rect;
        GetWindowRect(hwnd, &rect);

        WritePrivateProfileString(L"Window", L"Left", std::to_wstring(rect.left).c_str(), m_fileName.c_str());
        WritePrivateProfileString(L"Window", L"Top", std::to_wstring(rect.top).c_str(), m_fileName.c_str());
        WritePrivateProfileString(L"Window", L"Width", std::to_wstring(rect.right - rect.left).c_str(), m_fileName.c_str());
        WritePrivateProfileString(L"Window", L"Height", std::to_wstring(rect.bottom - rect.top).c_str(), m_fileName.c_str());
    }

    void RestoreWindowState(HWND hwnd) {
        wchar_t szLeft[10], szTop[10], szWidth[10], szHeight[10];

        GetPrivateProfileString(L"Window", L"Left", L"0", szLeft, sizeof(szLeft) / sizeof(wchar_t), m_fileName.c_str());
        GetPrivateProfileString(L"Window", L"Top", L"0", szTop, sizeof(szTop) / sizeof(wchar_t), m_fileName.c_str());
        GetPrivateProfileString(L"Window", L"Width", L"800", szWidth, sizeof(szWidth) / sizeof(wchar_t), m_fileName.c_str());
        GetPrivateProfileString(L"Window", L"Height", L"600", szHeight, sizeof(szHeight) / sizeof(wchar_t), m_fileName.c_str());

        int left = std::stoi(szLeft);
        int top = std::stoi(szTop);
        int width = std::stoi(szWidth);
        int height = std::stoi(szHeight);

        SetWindowPos(hwnd, NULL, left, top, width, height, SWP_NOZORDER);
    }

private:
    std::wstring m_fileName;
};
