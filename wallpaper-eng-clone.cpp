#ifndef UNICODE
#define UNICODE
#endif 
#define _CRT_SECURE_NO_WARNINGS
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

void loadImages(HWND hwnd);
void sus();
void ChangeUserDesktopWallpaper();
void opendabox(HWND hwnd);
void AddImage(HWND hwnd);

HBITMAP hImage;
HWND image;
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
            0, 0, 100, 25, // x, y, width, height
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
        if (LOWORD(wParam) == 2) {
            loadImages(hwnd);
        }
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void loadImages(HWND hwnd) {
    wchar_t filePath[256]; // Buffer to hold the file path
    for (int i{ 1 }; i < 873; i++) {
        if (i < 10) {
            swprintf(filePath, 256, L"C:/logs/frame_000%d.bmp", i); // Add frame number to path
        }
        else if (i < 100) {
            swprintf(filePath, 256, L"C:/logs/frame_00%d.bmp", i); // Add frame number to path
        }
        else {
            swprintf(filePath, 256, L"C:/logs/frame_0%d.bmp", i); // Add frame number to path
        }

        hImage = (HBITMAP)LoadImageW(NULL, filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (hImage == NULL) {
            MessageBox(NULL, L"Failed to load image", L"ERROR", MB_OK);
        }
        else {
        AddImage(hwnd);
            
        }
        

    }
}


void AddImage(HWND hwnd) {
    HWND image = CreateWindowW(L"Static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP, 0, 0, 100, 100, hwnd, NULL, NULL, NULL);
    DWORD sleep = SleepEx(13, FALSE);
    if (sleep == 0) {
        SendMessageW(image, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImage);
    }
}

void sus() {
    std::string videoFilePath = "C:/beta.mp4";
    std::string outputFramePath = "C:/logs/frame_%04d.bmp";

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
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST,     // ExStyle to prevent showing in taskbar
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

    if (hwnd == NULL) {
        return 0;
    }
    ShowWindow(hwnd, SW_MAXIMIZE);
    HWND taskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (taskbar) {
        ShowWindow(taskbar, SW_SHOW);
        SetWindowPos(taskbar, HWND_TOPMOST - 1, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }
    SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
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
                        ChangeUserDesktopWallpaper();
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





void ChangeUserDesktopWallpaper() {
   
    
    char narrowPath[256];

    for (int i{ 1 }; i < 873; i++) {
        // Format the path for the current frame
        if (i < 10) {
            std::sprintf(narrowPath, "C:/logs/frame_000%d.png", i);
        }
        else if (i < 100) {
            std::sprintf(narrowPath, "C:/logs/frame_00%d.png", i);
        }
        else {
            std::sprintf(narrowPath, "C:/logs/frame_0%d.png", i);
        }

        // Convert narrow string to wide string (UTF-16) for Windows API
      //  wchar_t widePath[MAX_PATH];
        //MultiByteToWideChar(CP_ACP, 0, narrowPath, -1, widePath, MAX_PATH);

        // Change the wallpaper
       // BOOL result = SystemParametersInfoW(
        //    SPI_SETDESKWALLPAPER,  // Action to perform
         //   0,                     // Not used, should be 0
          //  (void*)widePath,       // Path to wallpaper
           // SPIF_UPDATEINIFILE | SPIF_SENDCHANGE  // Flags to update and notify
       // );

//        if (!result) {
  //          std::cerr << "Failed to change wallpaper to: " << narrowPath << std::endl;
    //    }
        std::cout << "YO";
        // Sleep to control frame rate: 1000 ms / 30 frames = ~33 ms per frame
       // std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

      //  if (result)
       // {
        //    MessageBox(NULL, L"Wallpaper changed successfully!", L"Success", MB_OK);
       // }
       // else
        //{
         //   MessageBox(NULL, L"Failed to change wallpaper.", L"Error", MB_OK | MB_ICONERROR);
       // }

}

