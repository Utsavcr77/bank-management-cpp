#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Utils {
    // Input helpers
    double getDoubleInput(const std::string& prompt);
    int getIntInput(const std::string& prompt);
    std::string getStringInput(const std::string& prompt);
    std::string getHiddenInput(const std::string& prompt); // Masks PIN entry

    // Validation
    bool isNumeric(const std::string& str);
    bool isAlpha(const std::string& str);
    std::string trim(const std::string& str);
    std::string toUpperCase(const std::string& str);

    // Formatting
    std::string formatCurrency(double amount);
    std::string getCurrentDateTime();
    std::string getCurrentDate();
    void printBox(const std::string& text, int width = 60);

    // Simple hash (for PIN security)
    std::string simpleHash(const std::string& input);

    // Pause
    void pressEnterToContinue();
}

#endif // UTILS_H
