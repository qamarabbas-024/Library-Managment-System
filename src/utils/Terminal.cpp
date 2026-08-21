#include "utils/Terminal.hpp"
#include "utils/StringUtils.hpp"
#include <limits>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace LMS::Utils {

void Terminal::init() {
#ifdef _WIN32
    // Enable ANSI escape sequences in Windows console
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
}

void Terminal::clear() {
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[1;1H" << std::flush;
#endif
}

void Terminal::pause(const std::string& message) {
    std::cout << "\n" << Color::Dim << message << Color::Reset << std::flush;
    std::string dummy;
    std::getline(std::cin, dummy);
}

int Terminal::readInt(const std::string& prompt, int minVal, int maxVal, int defaultVal) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cin.clear();
            continue;
        }
        line = StringUtils::trim(line);
        if (line.empty() && defaultVal != -1) {
            return defaultVal;
        }
        try {
            size_t idx = 0;
            int val = std::stoi(line, &idx);
            if (idx == line.length() && val >= minVal && val <= maxVal) {
                return val;
            }
        } catch (...) {}
        std::cout << Color::Red << "  [!] Please enter a valid number between " 
                  << minVal << " and " << maxVal << "." << Color::Reset << "\n";
    }
}

double Terminal::readDouble(const std::string& prompt, double minVal, double maxVal) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cin.clear();
            continue;
        }
        line = StringUtils::trim(line);
        try {
            size_t idx = 0;
            double val = std::stod(line, &idx);
            if (idx == line.length() && val >= minVal && val <= maxVal) {
                return val;
            }
        } catch (...) {}
        std::cout << Color::Red << "  [!] Please enter a valid amount between " 
                  << minVal << " and " << maxVal << "." << Color::Reset << "\n";
    }
}

std::string Terminal::readString(const std::string& prompt, bool allowEmpty, const std::string& defaultVal) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cin.clear();
            continue;
        }
        line = StringUtils::trim(line);
        if (line.empty()) {
            if (!defaultVal.empty()) return defaultVal;
            if (allowEmpty) return "";
            std::cout << Color::Red << "  [!] Input cannot be empty. Please try again." << Color::Reset << "\n";
            continue;
        }
        return line;
    }
}

std::string Terminal::readPassword(const std::string& prompt) {
    std::cout << prompt;
    std::string password;
#ifdef _WIN32
    char ch;
    while ((ch = static_cast<char>(_getch())) != '\r') {
        if (ch == '\b') {
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b";
            }
        } else if (ch >= 32 && ch <= 126) {
            password.push_back(ch);
            std::cout << '*';
        }
    }
    std::cout << "\n";
#else
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    std::getline(std::cin, password);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    std::cout << "\n";
#endif
    return password;
}

bool Terminal::readConfirmation(const std::string& prompt, bool defaultYes) {
    std::string hint = defaultYes ? " [Y/n]: " : " [y/N]: ";
    std::cout << prompt << hint;
    std::string line;
    std::getline(std::cin, line);
    line = StringUtils::toLower(StringUtils::trim(line));
    if (line.empty()) return defaultYes;
    return (line == "y" || line == "yes");
}

void Terminal::printHeader(const std::string& title, const std::string& subtitle) {
    std::cout << Color::Cyan << Color::Bold;
    std::cout << "===============================================================================\n";
    std::cout << "  " << title << "\n";
    if (!subtitle.empty()) {
        std::cout << "  " << Color::Reset << Color::Dim << subtitle << Color::Cyan << Color::Bold << "\n";
    }
    std::cout << "===============================================================================\n" << Color::Reset;
}

void Terminal::printSuccess(const std::string& message) {
    std::cout << Color::Green << Color::Bold << "  [+] " << Color::Reset << Color::Green << message << Color::Reset << "\n";
}

void Terminal::printError(const std::string& message) {
    std::cout << Color::Red << Color::Bold << "  [-] " << Color::Reset << Color::Red << message << Color::Reset << "\n";
}

void Terminal::printWarning(const std::string& message) {
    std::cout << Color::Yellow << Color::Bold << "  [!] " << Color::Reset << Color::Yellow << message << Color::Reset << "\n";
}

void Terminal::printInfo(const std::string& message) {
    std::cout << Color::Cyan << "  [*] " << message << Color::Reset << "\n";
}

void Terminal::printDivider(int width, char ch) {
    std::cout << Color::Gray << std::string(width, ch) << Color::Reset << "\n";
}

} // namespace LMS::Utils
