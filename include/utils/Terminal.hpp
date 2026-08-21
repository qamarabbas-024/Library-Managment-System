#pragma once

#include <string>
#include <iostream>

namespace LMS::Utils {

namespace Color {
    inline const std::string Reset   = "\033[0m";
    inline const std::string Bold    = "\033[1m";
    inline const std::string Dim     = "\033[2m";
    inline const std::string Red     = "\033[31m";
    inline const std::string Green   = "\033[32m";
    inline const std::string Yellow  = "\033[33m";
    inline const std::string Blue    = "\033[34m";
    inline const std::string Magenta = "\033[35m";
    inline const std::string Cyan    = "\033[36m";
    inline const std::string White   = "\033[37m";
    inline const std::string Gray    = "\033[90m";
    inline const std::string BgBlue  = "\033[44m";
    inline const std::string BgGreen = "\033[42m";
}

class Terminal {
public:
    static void init();
    static void clear();
    static void pause(const std::string& message = "Press Enter to continue...");
    
    // Robust input handlers that prevent cin crashes & infinite loops
    static int readInt(const std::string& prompt, int minVal, int maxVal, int defaultVal = -1);
    static double readDouble(const std::string& prompt, double minVal, double maxVal);
    static std::string readString(const std::string& prompt, bool allowEmpty = false, const std::string& defaultVal = "");
    static std::string readPassword(const std::string& prompt);
    static bool readConfirmation(const std::string& prompt, bool defaultYes = true);

    // Visual formatting
    static void printHeader(const std::string& title, const std::string& subtitle = "");
    static void printSuccess(const std::string& message);
    static void printError(const std::string& message);
    static void printWarning(const std::string& message);
    static void printInfo(const std::string& message);
    static void printDivider(int width = 75, char ch = '-');
};

} // namespace LMS::Utils
