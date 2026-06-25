#include "Bank.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

// ─── Constructor / Destructor ─────────────────────────────────────────────────

Bank::Bank(const std::string& filePath)
    : dataFilePath(filePath), nextAccountNumber(100001) {
    loadFromFile();
}

Bank::~Bank() {
    saveToFile();
}

// ─── File I/O ─────────────────────────────────────────────────────────────────

void Bank::loadFromFile() {
    std::ifstream file(dataFilePath);
    if (!file.is_open()) return;

    std::string line;
    // First line: next account number
    if (std::getline(file, line) && !line.empty()) {
        try { nextAccountNumber = std::stoi(line); } catch (...) {}
    }
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        Account acc = Account::deserialize(line);
        if (acc.getAccountNumber() != 0) {
            accounts[acc.getAccountNumber()] = acc;
        }
    }
    file.close();
}

void Bank::saveToFile() const {
    std::ofstream file(dataFilePath);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Cannot write to data file.\n";
        return;
    }
    file << nextAccountNumber << "\n";
    for (const auto& pair : accounts) {
        file << pair.second.serialize() << "\n";
    }
    file.close();
}

// ─── Utility ──────────────────────────────────────────────────────────────────

void Bank::clearScreen() const {
    system(CLEAR);
}

void Bank::printLine(char ch, int width) const {
    std::cout << "  " << std::string(width, ch) << "\n";
}

void Bank::printHeader(const std::string& title) const {
    clearScreen();
    printLine('=');
    int pad = (60 - (int)title.size()) / 2;
    std::cout << "  " << std::string(pad, ' ') << title << "\n";
    printLine('=');
    std::cout << "\n";
}

int Bank::generateAccountNumber() {
    return nextAccountNumber++;
}

bool Bank::isValidAmount(double amount) const {
    return amount > 0 && amount <= 10000000.0;
}

bool Bank::isValidPin(const std::string& pin) const {
    return pin.size() == 4 && Utils::isNumeric(pin);
}

bool Bank::isValidName(const std::string& name) const {
    return name.size() >= 2 && name.size() <= 40;
}

// ─── Account Management ───────────────────────────────────────────────────────

int Bank::createAccount(const std::string& name, const std::string& pin,
                        const std::string& type, double initialDeposit) {
    if (!isValidName(name)) return -1;
    if (!isValidPin(pin)) return -2;
    if (initialDeposit < 0) return -3;

    int accNum = generateAccountNumber();
    accounts[accNum] = Account(accNum, name, pin, type, initialDeposit);
    saveToFile();
    return accNum;
}

bool Bank::deleteAccount(int accountNumber, const std::string& pin) {
    auto it = accounts.find(accountNumber);
    if (it == accounts.end()) return false;
    if (!it->second.verifyPin(pin)) return false;
    it->second.setIsActive(false);
    saveToFile();
    return true;
}

Account* Bank::findAccount(int accountNumber) {
    auto it = accounts.find(accountNumber);
    if (it != accounts.end() && it->second.getIsActive()) {
        return &it->second;
    }
    return nullptr;
}

// ─── Banking Operations ───────────────────────────────────────────────────────

bool Bank::deposit(int accountNumber, double amount, const std::string& pin) {
    Account* acc = findAccount(accountNumber);
    if (!acc || !acc->verifyPin(pin)) return false;
    if (!isValidAmount(amount)) return false;
    bool ok = acc->deposit(amount);
    if (ok) saveToFile();
    return ok;
}

bool Bank::withdraw(int accountNumber, double amount, const std::string& pin) {
    Account* acc = findAccount(accountNumber);
    if (!acc || !acc->verifyPin(pin)) return false;
    if (!isValidAmount(amount)) return false;
    bool ok = acc->withdraw(amount);
    if (ok) saveToFile();
    return ok;
}

bool Bank::transfer(int fromAccount, int toAccount, double amount, const std::string& pin) {
    Account* from = findAccount(fromAccount);
    Account* to   = findAccount(toAccount);
    if (!from || !to) return false;
    if (!from->verifyPin(pin)) return false;
    if (!isValidAmount(amount)) return false;
    if (from->getBalance() < amount) return false;

    from->withdraw(amount);
    to->deposit(amount);
    from->addTransaction("TRANSFER_OUT", amount, from->getBalance());
    to->addTransaction("TRANSFER_IN", amount, to->getBalance());
    saveToFile();
    return true;
}

bool Bank::checkBalance(int accountNumber, const std::string& pin) const {
    auto it = accounts.find(accountNumber);
    if (it == accounts.end() || !it->second.getIsActive()) return false;
    return it->second.verifyPin(pin);
}

// ─── Display Operations ───────────────────────────────────────────────────────

void Bank::displayAllAccounts() const {
    printHeader("ALL ACCOUNTS");
    if (accounts.empty()) {
        std::cout << "  No accounts found.\n";
        return;
    }
    std::cout << "  " << std::left
              << std::setw(12) << "Acc No."
              << std::setw(22) << "Holder Name"
              << std::setw(12) << "Type"
              << std::setw(14) << "Balance"
              << std::setw(8)  << "Status" << "\n";
    printLine('-');
    for (const auto& pair : accounts) {
        const Account& a = pair.second;
        std::cout << "  "
                  << std::setw(12) << a.getAccountNumber()
                  << std::setw(22) << a.getHolderName()
                  << std::setw(12) << a.getAccountType()
                  << std::setw(14) << Utils::formatCurrency(a.getBalance())
                  << std::setw(8)  << (a.getIsActive() ? "Active" : "Closed") << "\n";
    }
    printLine('-');
}

void Bank::displayAccountDetails(int accountNumber, const std::string& pin) const {
    auto it = accounts.find(accountNumber);
    if (it == accounts.end() || !it->second.getIsActive()) {
        std::cout << "\n  [!] Account not found.\n";
        return;
    }
    if (!it->second.verifyPin(pin)) {
        std::cout << "\n  [!] Incorrect PIN.\n";
        return;
    }
    it->second.displayAccountInfo();
}

void Bank::displayTransactionHistory(int accountNumber, const std::string& pin) const {
    auto it = accounts.find(accountNumber);
    if (it == accounts.end() || !it->second.getIsActive()) {
        std::cout << "\n  [!] Account not found.\n";
        return;
    }
    if (!it->second.verifyPin(pin)) {
        std::cout << "\n  [!] Incorrect PIN.\n";
        return;
    }
    it->second.displayTransactionHistory();
}

bool Bank::changePin(int accountNumber, const std::string& oldPin, const std::string& newPin) {
    Account* acc = findAccount(accountNumber);
    if (!acc) return false;
    if (!isValidPin(newPin)) return false;
    bool ok = acc->changePin(oldPin, newPin);
    if (ok) saveToFile();
    return ok;
}

void Bank::displayBankSummary() const {
    printHeader("BANK SUMMARY");
    int total = 0, active = 0;
    double totalBalance = 0;
    for (const auto& pair : accounts) {
        total++;
        if (pair.second.getIsActive()) {
            active++;
            totalBalance += pair.second.getBalance();
        }
    }
    std::cout << "  Total Accounts   : " << total << "\n";
    std::cout << "  Active Accounts  : " << active << "\n";
    std::cout << "  Closed Accounts  : " << (total - active) << "\n";
    std::cout << "  Total Deposits   : " << Utils::formatCurrency(totalBalance) << "\n";
    printLine('-');
}

// ─── Menus ────────────────────────────────────────────────────────────────────

void Bank::accountMenu(int accountNumber) {
    Account* acc = findAccount(accountNumber);
    if (!acc) return;

    while (true) {
        printHeader("ACCOUNT MENU — #" + std::to_string(accountNumber));
        std::cout << "  Welcome, " << acc->getHolderName() << "!\n\n";
        std::cout << "  [1] Check Balance\n";
        std::cout << "  [2] Deposit\n";
        std::cout << "  [3] Withdraw\n";
        std::cout << "  [4] Transfer Funds\n";
        std::cout << "  [5] Transaction History\n";
        std::cout << "  [6] Account Details\n";
        std::cout << "  [7] Change PIN\n";
        std::cout << "  [0] Logout\n\n";
        printLine('-');

        int choice = Utils::getIntInput("  Enter choice: ");

        if (choice == 0) {
            std::cout << "\n  Logged out successfully.\n";
            Utils::pressEnterToContinue();
            return;
        }

        std::string pin;

        switch (choice) {
        case 1: {
            pin = Utils::getHiddenInput("  Enter PIN: ");
            if (acc->verifyPin(pin)) {
                std::cout << "\n  Current Balance: " << Utils::formatCurrency(acc->getBalance()) << "\n";
            } else {
                std::cout << "\n  [!] Incorrect PIN.\n";
            }
            Utils::pressEnterToContinue();
            break;
        }
        case 2: {
            double amount = Utils::getDoubleInput("  Enter deposit amount: Rs. ");
            pin = Utils::getHiddenInput("  Enter PIN: ");
            if (deposit(accountNumber, amount, pin)) {
                std::cout << "\n  [+] Deposited " << Utils::formatCurrency(amount) << " successfully!\n";
                std::cout << "  New Balance: " << Utils::formatCurrency(acc->getBalance()) << "\n";
            } else {
                std::cout << "\n  [!] Deposit failed. Check PIN or amount.\n";
            }
            Utils::pressEnterToContinue();
            break;
        }
        case 3: {
            double amount = Utils::getDoubleInput("  Enter withdrawal amount: Rs. ");
            pin = Utils::getHiddenInput("  Enter PIN: ");
            if (withdraw(accountNumber, amount, pin)) {
                std::cout << "\n  [-] Withdrawn " << Utils::formatCurrency(amount) << " successfully!\n";
                std::cout << "  Remaining Balance: " << Utils::formatCurrency(acc->getBalance()) << "\n";
            } else {
                std::cout << "\n  [!] Withdrawal failed. Check PIN, amount, or balance.\n";
            }
            Utils::pressEnterToContinue();
            break;
        }
        case 4: {
            int toAcc = Utils::getIntInput("  Enter destination account number: ");
            double amount = Utils::getDoubleInput("  Enter transfer amount: Rs. ");
            pin = Utils::getHiddenInput("  Enter PIN: ");
            if (transfer(accountNumber, toAcc, amount, pin)) {
                std::cout << "\n  [->] Transferred " << Utils::formatCurrency(amount)
                          << " to account #" << toAcc << " successfully!\n";
            } else {
                std::cout << "\n  [!] Transfer failed. Check PIN, amount, balance, or destination.\n";
            }
            Utils::pressEnterToContinue();
            break;
        }
        case 5: {
            pin = Utils::getHiddenInput("  Enter PIN: ");
            displayTransactionHistory(accountNumber, pin);
            Utils::pressEnterToContinue();
            break;
        }
        case 6: {
            pin = Utils::getHiddenInput("  Enter PIN: ");
            displayAccountDetails(accountNumber, pin);
            Utils::pressEnterToContinue();
            break;
        }
        case 7: {
            std::string oldPin = Utils::getHiddenInput("  Enter current PIN: ");
            std::string newPin = Utils::getHiddenInput("  Enter new 4-digit PIN: ");
            std::string confirmPin = Utils::getHiddenInput("  Confirm new PIN: ");
            if (newPin != confirmPin) {
                std::cout << "\n  [!] PINs do not match.\n";
            } else if (changePin(accountNumber, oldPin, newPin)) {
                std::cout << "\n  [+] PIN changed successfully!\n";
            } else {
                std::cout << "\n  [!] Failed. Check current PIN and ensure new PIN is 4 digits.\n";
            }
            Utils::pressEnterToContinue();
            break;
        }
        default:
            std::cout << "\n  [!] Invalid option.\n";
            Utils::pressEnterToContinue();
        }
    }
}

void Bank::adminMenu() {
    const std::string ADMIN_PASS = "admin123";
    printHeader("ADMIN LOGIN");
    std::string pass = Utils::getHiddenInput("  Enter admin password: ");
    if (pass != ADMIN_PASS) {
        std::cout << "\n  [!] Access denied.\n";
        Utils::pressEnterToContinue();
        return;
    }

    while (true) {
        printHeader("ADMIN PANEL");
        std::cout << "  [1] View All Accounts\n";
        std::cout << "  [2] Bank Summary\n";
        std::cout << "  [3] Close an Account\n";
        std::cout << "  [0] Back\n\n";
        printLine('-');

        int choice = Utils::getIntInput("  Enter choice: ");
        if (choice == 0) return;

        switch (choice) {
        case 1:
            displayAllAccounts();
            Utils::pressEnterToContinue();
            break;
        case 2:
            displayBankSummary();
            Utils::pressEnterToContinue();
            break;
        case 3: {
            int accNum = Utils::getIntInput("  Enter account number to close: ");
            std::string pin = Utils::getHiddenInput("  Enter account PIN to confirm: ");
            if (deleteAccount(accNum, pin)) {
                std::cout << "\n  [+] Account #" << accNum << " closed.\n";
            } else {
                std::cout << "\n  [!] Failed. Check account number and PIN.\n";
            }
            Utils::pressEnterToContinue();
            break;
        }
        default:
            std::cout << "\n  [!] Invalid option.\n";
            Utils::pressEnterToContinue();
        }
    }
}

void Bank::mainMenu() {
    while (true) {
        printHeader("SECURE BANK MANAGEMENT SYSTEM");
        std::cout << "  [1] Open New Account\n";
        std::cout << "  [2] Login to Account\n";
        std::cout << "  [3] Admin Panel\n";
        std::cout << "  [0] Exit\n\n";
        printLine('=');

        int choice = Utils::getIntInput("  Enter choice: ");

        switch (choice) {
        case 0:
            printHeader("GOODBYE");
            std::cout << "  Thank you for banking with us!\n\n";
            return;

        case 1: {
            printHeader("OPEN NEW ACCOUNT");
            std::string name = Utils::getStringInput("  Full Name       : ");
            if (!isValidName(name)) {
                std::cout << "\n  [!] Name must be 2-40 characters.\n";
                Utils::pressEnterToContinue(); break;
            }
            std::cout << "  Account Type:\n";
            std::cout << "  [1] Savings\n  [2] Current\n";
            int typeChoice = Utils::getIntInput("  Choose: ");
            std::string type = (typeChoice == 2) ? "CURRENT" : "SAVINGS";

            std::string pin = Utils::getHiddenInput("  Set 4-digit PIN : ");
            if (!isValidPin(pin)) {
                std::cout << "\n  [!] PIN must be exactly 4 digits.\n";
                Utils::pressEnterToContinue(); break;
            }
            std::string pin2 = Utils::getHiddenInput("  Confirm PIN     : ");
            if (pin != pin2) {
                std::cout << "\n  [!] PINs do not match.\n";
                Utils::pressEnterToContinue(); break;
            }
            double initial = Utils::getDoubleInput("  Initial Deposit : Rs. ");

            int accNum = createAccount(name, pin, type, initial);
            if (accNum > 0) {
                std::cout << "\n  ╔══════════════════════════════════════╗\n";
                std::cout << "  ║   Account Created Successfully!      ║\n";
                std::cout << "  ║   Your Account Number: " << std::left
                          << std::setw(13) << accNum << "║\n";
                std::cout << "  ║   Please save this number safely.    ║\n";
                std::cout << "  ╚══════════════════════════════════════╝\n";
            } else {
                std::cout << "\n  [!] Account creation failed.\n";
            }
            Utils::pressEnterToContinue();
            break;
        }

        case 2: {
            printHeader("ACCOUNT LOGIN");
            int accNum = Utils::getIntInput("  Account Number: ");
            Account* acc = findAccount(accNum);
            if (!acc) {
                std::cout << "\n  [!] Account not found or inactive.\n";
                Utils::pressEnterToContinue(); break;
            }
            std::string pin = Utils::getHiddenInput("  Enter PIN     : ");
            if (!acc->verifyPin(pin)) {
                std::cout << "\n  [!] Incorrect PIN. Access denied.\n";
                Utils::pressEnterToContinue(); break;
            }
            accountMenu(accNum);
            break;
        }

        case 3:
            adminMenu();
            break;

        default:
            std::cout << "\n  [!] Invalid choice. Try again.\n";
            Utils::pressEnterToContinue();
        }
    }
}
