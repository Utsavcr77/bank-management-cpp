#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <vector>

struct Transaction {
    std::string type;       // "DEPOSIT", "WITHDRAWAL", "TRANSFER"
    double amount;
    std::string date;
    double balanceAfter;
};

class Account {
private:
    int accountNumber;
    std::string holderName;
    std::string pin;         // Stored as hashed string
    double balance;
    std::string accountType; // "SAVINGS" or "CURRENT"
    std::string createdDate;
    bool isActive;
    std::vector<Transaction> transactionHistory;

    std::string hashPin(const std::string& rawPin) const;
    std::string getCurrentDate() const;

public:
    // Constructors
    Account();
    Account(int accNum, const std::string& name, const std::string& rawPin,
            const std::string& type, double initialBalance = 0.0);

    // Getters
    int getAccountNumber() const;
    std::string getHolderName() const;
    double getBalance() const;
    std::string getAccountType() const;
    std::string getCreatedDate() const;
    bool getIsActive() const;
    const std::vector<Transaction>& getTransactionHistory() const;

    // Setters
    void setHolderName(const std::string& name);
    void setBalance(double amount);
    void setIsActive(bool status);

    // Banking Operations
    bool deposit(double amount);
    bool withdraw(double amount);
    bool verifyPin(const std::string& rawPin) const;
    bool changePin(const std::string& oldPin, const std::string& newPin);
    void addTransaction(const std::string& type, double amount, double balAfter);

    // Display
    void displayAccountInfo() const;
    void displayTransactionHistory() const;

    // Serialization (for file storage)
    std::string serialize() const;
    static Account deserialize(const std::string& data);
};

#endif // ACCOUNT_H
