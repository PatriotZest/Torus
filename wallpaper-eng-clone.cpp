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

// ----------------------------------------------------------------------------------------
std::mutex videoMutex;
void DecodeAndRenderVideo(const std::string& filename, HWND hwnd) {
    std::lock_guard<std::mutex> lock(videoMutex);
    std::cout << "Opening video file..." << std::endl;
    if (avformat_open_input(&pFormatCtx, filename.c_str(), NULL, NULL) != 0) {
        std::cerr << "Couldn't open video file" << std::endl;
        return;
    }

    std::cout << "Retrieving stream information..." << std::endl;
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        std::cerr << "Couldn't find stream information" << std::endl;
        return;
    }

    std::cout << "Finding video stream..." << std::endl;
    for (unsigned int i = 0; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex == -1) {
        std::cerr << "Couldn't find a video stream" << std::endl;
        return;
    }

    std::cout << "Initializing codec context..." << std::endl;
    AVCodecParameters* pCodecParams = pFormatCtx->streams[videoStreamIndex]->codecpar;
    const AVCodec* pCodec = avcodec_find_decoder(pCodecParams->codec_id);
    pCodecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecCtx, pCodecParams);

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        std::cerr << "Couldn't open codec" << std::endl;
        return;
    }

    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();

    if (!pFrame || !pFrameRGB) {
        std::cerr << "Couldn't allocate video frame" << std::endl;
        return;
    }

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1);

    sws_ctx = sws_getContext(
        pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
        pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, NULL, NULL, NULL);

    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = pCodecCtx->width;
    bitmapInfo.bmiHeader.biHeight = -pCodecCtx->height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 24;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    bitmapInfo.bmiHeader.biSizeImage = 0;
    bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    bitmapInfo.bmiHeader.biClrUsed = 0;
    bitmapInfo.bmiHeader.biClrImportant = 0;

    std::cout << "Decoding and rendering video frames..." << std::endl;
    AVPacket packet;
    while (av_read_frame(pFormatCtx, &packet) >= 0) {
        if (packet.stream_index == videoStreamIndex) {
            if (avcodec_send_packet(pCodecCtx, &packet) < 0) {
                std::cerr << "Error sending packet for decoding" << std::endl;
                continue;
            }

            while (avcodec_receive_frame(pCodecCtx, pFrame) >= 0) {
                sws_scale(
                    sws_ctx,
                    pFrame->data,
                    pFrame->linesize,
                    0,
                    pCodecCtx->height,
                    pFrameRGB->data,
                    pFrameRGB->linesize);

                HDC hdc = GetDC(hwnd);
                RenderFrame(hdc, pFrameRGB);
                ReleaseDC(hwnd, hdc);

                std::this_thread::sleep_for(std::chrono::milliseconds(40));
            }
        }

        av_packet_unref(&packet);
    }

    std::cout << "Cleaning up..." << std::endl;
    av_frame_free(&pFrame);
    av_frame_free(&pFrameRGB);
    avcodec_free_context(&pCodecCtx);
    avformat_close_input(&pFormatCtx);
    sws_freeContext(sws_ctx);
}






void RenderFrame(HDC hdc, AVFrame* frame) {
    // Render the RGB frame to the window using GDI
    SetDIBitsToDevice(
        hdc,
        0, 0,                         // X, Y position on the window
        pCodecCtx->width, pCodecCtx->height, // Width and height of the image
        0, 0,                         // X, Y position on the image (top-left corner)
        0, pCodecCtx->height,         // Start scan line, number of scan lines
        frame->data[0],               // Pointer to the image bits
        &bitmapInfo,                  // Pointer to the BITMAPINFO structure
        DIB_RGB_COLORS);              // Use RGB for the color data
}

