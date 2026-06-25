#include "Bank.h"
#include <iostream>
#include <filesystem>

int main() {
    // Ensure data directory exists
    std::filesystem::create_directories("data");

    Bank bank("data/bank_data.txt");
    bank.mainMenu();

    return 0;
}
