#ifndef UNICODE
#define UNICODE
#endif 
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include "wallpaper-eng-clone.h"
#include <commctrl.h>
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <shobjidl.h>  // for COM
#include <cstdlib>
#include <chrono>
#include <thread>
#include <synchapi.h>
#include <regex>
#include <filesystem>
#include <algorithm>
#include <sstream>




void loadImages(HWND hwnd, int frameCount);
void sus(PWSTR videoPath, HWND hwnd);
void opendabox(HWND hwnd);
void AddImage(HWND hwnd, HBITMAP hImage);
std::string ConvertPWSTRToString(PWSTR pWideStr);

PWSTR pszFilePath;
HBITMAP hImage;
HWND image;
bool isControlShiftAlt() {
    return (GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
        (GetAsyncKeyState(VK_SHIFT) & 0x8000) &&
        (GetAsyncKeyState(VK_MENU) & 0x8000);
}


namespace fs = std::filesystem;
int getTotalFrameCount(const std::string& directory) {
    std::regex framePattern("frame_(\\d{4})\\.bmp"); // Regex to match files like frame_0001.bmp
    int highestFrameNumber = 0;

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::smatch match;
            std::string filename = entry.path().filename().string();

            if (std::regex_search(filename, match, framePattern)) {
                int frameNumber = std::stoi(match[1]);  // Extract the number
                highestFrameNumber = std::max(highestFrameNumber, frameNumber);
            }
        }
    }

    return highestFrameNumber;
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HWND hButton;

    switch (uMsg)
    {
    case WM_CREATE:
        hButton = CreateWindowEx(
            0, L"BUTTON", L"Open File",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            300, 300, 100, 25, // x, y, width, height
            hwnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL
        );

        if (hButton == NULL) {
            MessageBox(hwnd, L"Failed to create 'Open File' button", L"Error", MB_OK | MB_ICONERROR);
        }

        hButton = CreateWindowEx(
            0, L"BUTTON", L"Open Video",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            0, 0, 100, 25, // x, y, width, height
            hwnd, (HMENU)2, ((LPCREATESTRUCT)lParam)->hInstance, NULL
        );

        if (hButton == NULL) {
            MessageBox(hwnd, L"Failed to create 'Open Video' button", L"Error", MB_OK | MB_ICONERROR);
        }

        return 0;

    case WM_CLOSE:
        if (MessageBox(hwnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK)
        {

            DestroyWindow(hwnd);
            PostQuitMessage(0);
        }
        else {
            MessageBox(hwnd, L"You are a really funny person ngl", L"My app ofc", MB_OKCANCEL);
        }
        return 0;

    case WM_COMMAND:

        if (LOWORD(wParam) == 1) { // Button click
            opendabox(hwnd);
        }
        if (LOWORD(wParam) == 2) {
            //this
        }
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"SampleWindowClass";

    // Register the window class
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    // Create the window with WS_POPUP style (no title bar or borders)
    HWND hwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW,     // ExStyle to prevent showing in taskbar
        CLASS_NAME,           // Window class
        L"Main",              // Window text (optional)
        WS_POPUP,             // Window style (no border or title bar)
        0, 0,                 // Position
        GetSystemMetrics(SM_CXSCREEN),  // Width of screen
        GetSystemMetrics(SM_CYSCREEN),  // Height of screen
        NULL,                 // Parent window    
        NULL,                 // Menu
        hInstance,            // Instance handle
        NULL                  // Additional application data
    );

   
    ShowWindow(hwnd, SW_MAXIMIZE);

    SetWindowPos(hwnd, HWND_BOTTOM - 1, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // Check for Control + Shift + Alt shortcut outside of the message handler
        if (isControlShiftAlt()) {
            if (MessageBox(hwnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK) {
                PostQuitMessage(0);
            }
            else {
                MessageBox(hwnd, L"You are a really funny person ngl", L"My app ofc", MB_OK);
            }
        }
    }

    return 0;
}



void loadImages(HWND hwnd, int frameCount) {
    wchar_t filePath[256];


    HWND hButtonOpenFile = GetDlgItem(hwnd, 1);
    HWND hButtonOpenVideo = GetDlgItem(hwnd, 2);
    ShowWindow(hButtonOpenFile, SW_HIDE);
    ShowWindow(hButtonOpenVideo, SW_HIDE);
    while (true) {
        for (int i = 1; i < frameCount; i++) {
            if (i < 10) {
                swprintf(filePath, 256, L"Frames/frame_000%d.bmp", i);
            }
            else if (i < 100) {
                swprintf(filePath, 256, L"Frames/frame_00%d.bmp", i);
            }
            else if (i < 1000) {
                swprintf(filePath, 256, L"Frames/frame_0%d.bmp", i);
            }
            else {
                swprintf(filePath, 256, L"Frames/frame_%d.bmp", i);
            }

            // Load the bitmap
            HBITMAP hImage = (HBITMAP)LoadImageW(NULL, filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            if (hImage == NULL) {
                MessageBox(NULL, L"Failed to load image", L"ERROR", MB_OK);
                continue;
            }


            AddImage(hwnd, hImage);
            Sleep(13);
            DeleteObject(hImage);
        }
    }
    ShowWindow(hButtonOpenFile, SW_SHOW);
    ShowWindow(hButtonOpenVideo, SW_SHOW);


    // after ending part:
    HBITMAP endImage = (HBITMAP)LoadImageW(NULL, L"Assets/background.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    AddImage(hwnd, endImage);

}

void AddImage(HWND hwnd, HBITMAP hImage) {

    HWND image = CreateWindowW(L"Static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP, 0, 0, 100, 100, hwnd, NULL, NULL, NULL);
    if (image) {
        SendMessageW(image, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImage);
        Sleep(13);
        DestroyWindow(image);
    }
}

std::string ConvertPWSTRToString(PWSTR pWideStr) {
    // Get the length of the resulting multi-byte string
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, pWideStr, -1, NULL, 0, NULL, NULL);

    // Allocate a buffer for the converted string
    std::string result(bufferSize, '\0');

    // Perform the actual conversion
    WideCharToMultiByte(CP_UTF8, 0, pWideStr, -1, &result[0], bufferSize, NULL, NULL);

    // Remove the trailing null character added by WideCharToMultiByte
    result.resize(bufferSize - 1);

    return result;
}

bool DeleteFolderContents(const std::wstring& folderPath) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((folderPath + L"\\*").c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return false; // Folder not found or error
    }

    do {
        const std::wstring fileOrDirName = findFileData.cFileName;

        if (fileOrDirName != L"." && fileOrDirName != L"..") {
            const std::wstring fullPath = folderPath + L"\\" + fileOrDirName;

            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (!DeleteFolderContents(fullPath)) {
                    FindClose(hFind);
                    return false;
                }
                if (!RemoveDirectory(fullPath.c_str())) {
                    FindClose(hFind);
                    return false;
                }
            }
            else {
                if (!DeleteFile(fullPath.c_str())) {
                    FindClose(hFind);
                    return false;
                }
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
    return true;
}

void sus(PWSTR videoPath, HWND hwnd) {
    std::string videoFilePath = ConvertPWSTRToString(videoPath);
    std::wstring wstr(videoFilePath.begin(), videoFilePath.end());
    OutputDebugString(wstr.c_str());
    std::wstring logPath = L"Frames";
    if (!DeleteFolderContents(logPath)) {
        MessageBox(hwnd, L"Failed to clear old frames.", L"Torus", MB_OKCANCEL | MB_ICONERROR);
        return;
    }
    // Define output frame path and FFmpeg path
    std::string outputFramePath = "Frames/frame_%04d.bmp";
    std::string ffmpegPath = "ffmpeg.exe";

    // Construct the FFmpeg command
    std::string command = ffmpegPath + " -i \"" + videoFilePath + "\" \"" + outputFramePath + "\"";

    // Debug output for the command being executed
    OutputDebugStringA(command.c_str());

    // Execute the command and capture the result
    int result = std::system(command.c_str());

    // Check if the command executed successfully
    if (result == 0) {
        // Check if the frames were actually generated
        int frameCount = getTotalFrameCount("Frames");
        if (frameCount > 0) {
            loadImages(hwnd, frameCount);
        }
        else {
            MessageBox(hwnd, L"No frames were extracted. Please check the input video.", L"Torus", MB_OKCANCEL | MB_ICONERROR);
        }
    }
    else {
        // Construct a detailed error message
        std::ostringstream errorMsg;
        errorMsg << "Command execution failed with error code " << result;
        MessageBox(hwnd, std::wstring(errorMsg.str().begin(), errorMsg.str().end()).c_str(), L"Torus", MB_OKCANCEL | MB_ICONERROR);
    }
}


void opendabox(HWND hwnd) {
    HRESULT hr;
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);  // has to be null always

    if (SUCCEEDED(hr)) {
        IFileOpenDialog* pFileOpen;
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));


        if (SUCCEEDED(hr)) {
            // To show the box
            hr = pFileOpen->Show(hwnd);

            // to get file name from box
            if (SUCCEEDED(hr)) {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr)) {
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    if (SUCCEEDED(hr)) {
                        sus(pszFilePath, hwnd);
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
                pFileOpen->Release();
            }
            CoUninitialize();
        }
    }
}




