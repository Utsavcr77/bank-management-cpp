#ifndef BANK_H
#define BANK_H

#include "Account.h"
#include <map>
#include <string>

class Bank {
private:
    std::map<int, Account> accounts;  // accountNumber -> Account
    std::string dataFilePath;
    int nextAccountNumber;

    // File Operations
    void loadFromFile();
    void saveToFile() const;
    int generateAccountNumber();

    // Utility
    void clearScreen() const;
    void printLine(char ch = '-', int width = 60) const;
    void printHeader(const std::string& title) const;
    bool isValidAmount(double amount) const;
    bool isValidPin(const std::string& pin) const;
    bool isValidName(const std::string& name) const;

public:
    Bank(const std::string& filePath = "data/bank_data.txt");
    ~Bank();

    // Account Management
    int createAccount(const std::string& name, const std::string& pin,
                      const std::string& type, double initialDeposit);
    bool deleteAccount(int accountNumber, const std::string& pin);
    Account* findAccount(int accountNumber);

    // Banking Operations
    bool deposit(int accountNumber, double amount, const std::string& pin);
    bool withdraw(int accountNumber, double amount, const std::string& pin);
    bool transfer(int fromAccount, int toAccount, double amount, const std::string& pin);
    bool checkBalance(int accountNumber, const std::string& pin) const;

    // Display Operations
    void displayAllAccounts() const;
    void displayAccountDetails(int accountNumber, const std::string& pin) const;
    void displayTransactionHistory(int accountNumber, const std::string& pin) const;

    // PIN Management
    bool changePin(int accountNumber, const std::string& oldPin, const std::string& newPin);

    // Statistics
    void displayBankSummary() const;

    // Menu Systems
    void mainMenu();
    void accountMenu(int accountNumber);
    void adminMenu();
};

#endif // BANK_H
