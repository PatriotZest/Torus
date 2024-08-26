#ifndef UNICODE
#define UNICODE
#endif 
#include "wallpaper-eng-clone.h"
#include <commctrl.h>
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <shobjidl.h>  // for COM
void ChangeUserDesktopWallpaper(PWSTR wallpaper);
void opendabox(HWND hwnd);
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
            320, 10, 100, 25,
            hwnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL
        );
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
            opendabox(hwnd);
        }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        // Here you can add code to paint your window
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
    }
    return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
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
                        MessageBoxW(NULL, pszFilePath, L"File Path:", MB_OK);
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
   
    const wchar_t* wallpaperPath = wallpaper;

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
