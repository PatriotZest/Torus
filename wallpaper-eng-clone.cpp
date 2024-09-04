#ifndef UNICODE
#define UNICODE
#endif 
#include "wallpaper-eng-clone.h"
#include <commctrl.h>
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <shobjidl.h>  // for COM
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <cstdlib>



void sus();
void ChangeUserDesktopWallpaper(PWSTR wallpaper);
void opendabox(HWND hwnd);
void RenderFrame(HDC hdc, AVFrame* frame);



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HWND hButton;
    static AppState* appState = nullptr;
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
            600, 600, 100, 25, // x, y, width, height
            hwnd, (HMENU)2, ((LPCREATESTRUCT)lParam)->hInstance, NULL
        );

        if (hButton == NULL) {
            MessageBox(hwnd, L"Failed to create 'Open Video' button", L"Error", MB_OK | MB_ICONERROR);
        }

        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)((LPCREATESTRUCT)lParam)->hInstance);
        appState = new AppState(L"appsettings.ini");
        appState->RestoreWindowState(hwnd);
        return 0;

    case WM_CLOSE:
        if (MessageBox(hwnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK)
        {
            if (appState) {
                appState->SaveWindowState(hwnd);
            }
            DestroyWindow(hwnd);
            PostQuitMessage(0);
        }
        else {
            MessageBox(hwnd, L"You are a really funny person ngl", L"My app ofc", MB_OKCANCEL);
        }
        return 0;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) { // Button click
            sus();
        }
        else if (LOWORD(wParam) == 1) { // Open Video button
            HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            OpenVideoWindow(hwnd, hInstance, hPrevInstance, pCmdLine, nCmdShow);
        }
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void sus() {
    std::string videoFilePath = "C:/beta.mp4";
    std::string outputFramePath = "C:/logs/frame_%04d.png";

    // Construct the FFmpeg command with the bundled FFmpeg executable
    std::string ffmpegPath = "C:/ffmpeg-2024-08-26-git-98610fe95f-full_build/bin/ffmpeg.exe"; // Adjust for your platform (e.g., ffmpeg.exe on Windows)
    std::string command = ffmpegPath + " -i " + videoFilePath + " " + outputFramePath;

    // Execute the command
    int result = std::system(command.c_str());

    // Check the result
    if (result == 0) {
        std::cout << "Frames extracted successfully!" << std::endl;
    }
    else {
        std::cerr << "Failed to extract frames." << std::endl;
    }
}
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{

    char wallpaperPath[MAX_PATH];

    // Get the current desktop wallpaper path
    if (SystemParametersInfoA(SPI_GETDESKWALLPAPER, MAX_PATH, wallpaperPath, 0))
    {
        std::cout << "Current wallpaper: " << wallpaperPath << std::endl;
    }
    else
    {
        std::cerr << "Failed to retrieve current wallpaper." << std::endl;
    }
    
   
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.
     HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Learn to Program Windows",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );


    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
}
void VideoThreadFunction(HWND hwnd) {
    try {
        DecodeAndRenderVideo("C:/vid.mp4", hwnd);
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

    return 0;

}





void opendabox(HWND hwnd) {
    HRESULT hr;


    //Intializing the COM Library
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
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    if (SUCCEEDED(hr)) {
                        ChangeUserDesktopWallpaper(pszFilePath);
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





HWND GetDesktopWindowHandle() {
    HWND progman = FindWindow(L"Progman", NULL);
    HWND desktop = NULL;

    // Send message to Progman to spawn a WorkerW window
    SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, NULL);

    // Enumerate through WorkerW windows to find the one that has a child of SHELLDLL_DefView
    HWND workerw = NULL;
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        HWND p = FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", NULL);
        if (p != NULL) {
            *(HWND*)lParam = FindWindowEx(NULL, hwnd, L"WorkerW", NULL);
        }
        return true;
        }, (LPARAM)&workerw);

    if (workerw != NULL) {
        desktop = workerw;
    }

    return desktop;
}





void ChangeUserDesktopWallpaper(PWSTR wallpaper) {
   
    const char* narrowPath = "C:/logs/frame_0001.png";
    wchar_t widePath[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, narrowPath, -1, widePath, MAX_PATH);

    wchar_t* wallpaperPath = widePath;

    // Change the wallpaper
    BOOL result = SystemParametersInfoW(
        SPI_SETDESKWALLPAPER,  // Action to perform
        0,                     // Not used, should be 0
        (void*)wallpaperPath,  // Path to wallpaper
        SPIF_UPDATEINIFILE | SPIF_SENDCHANGE  // Flags to update and notify
    );

    if (result)
    {
        MessageBox(NULL, L"Wallpaper changed successfully!", L"Success", MB_OK);
    }
    else
    {
        MessageBox(NULL, L"Failed to change wallpaper.", L"Error", MB_OK | MB_ICONERROR);
    }

}
