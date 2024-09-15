#include <iostream>
#include <filesystem>
#include <string>
#include <regex>
#include <tchar.h>
#include <cstdlib>
#include <windows.h>
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