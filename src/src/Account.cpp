#include "Account.h"
#include "Utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// ─── Private Helpers ──────────────────────────────────────────────────────────

std::string Account::hashPin(const std::string& rawPin) const {
    return Utils::simpleHash(rawPin);
}

std::string Account::getCurrentDate() const {
    return Utils::getCurrentDate();
}

// ─── Constructors ─────────────────────────────────────────────────────────────

Account::Account()
    : accountNumber(0), holderName(""), pin(""), balance(0.0),
      accountType("SAVINGS"), createdDate(""), isActive(false) {}

Account::Account(int accNum, const std::string& name, const std::string& rawPin,
                 const std::string& type, double initialBalance)
    : accountNumber(accNum), holderName(name), balance(initialBalance),
      accountType(type), isActive(true) {
    pin = hashPin(rawPin);
    createdDate = getCurrentDate();
    if (initialBalance > 0) {
        addTransaction("INITIAL_DEPOSIT", initialBalance, initialBalance);
    }
}

// ─── Getters ──────────────────────────────────────────────────────────────────

int Account::getAccountNumber() const { return accountNumber; }
std::string Account::getHolderName() const { return holderName; }
double Account::getBalance() const { return balance; }
std::string Account::getAccountType() const { return accountType; }
std::string Account::getCreatedDate() const { return createdDate; }
bool Account::getIsActive() const { return isActive; }
const std::vector<Transaction>& Account::getTransactionHistory() const {
    return transactionHistory;
}

// ─── Setters ──────────────────────────────────────────────────────────────────

void Account::setHolderName(const std::string& name) { holderName = name; }
void Account::setBalance(double amount) { balance = amount; }
void Account::setIsActive(bool status) { isActive = status; }

// ─── Banking Operations ───────────────────────────────────────────────────────

bool Account::deposit(double amount) {
    if (amount <= 0) return false;
    balance += amount;
    addTransaction("DEPOSIT", amount, balance);
    return true;
}

bool Account::withdraw(double amount) {
    if (amount <= 0 || amount > balance) return false;
    balance -= amount;
    addTransaction("WITHDRAWAL", amount, balance);
    return true;
}

bool Account::verifyPin(const std::string& rawPin) const {
    return hashPin(rawPin) == pin;
}

bool Account::changePin(const std::string& oldPin, const std::string& newPin) {
    if (!verifyPin(oldPin)) return false;
    pin = hashPin(newPin);
    return true;
}

void Account::addTransaction(const std::string& type, double amount, double balAfter) {
    Transaction t;
    t.type        = type;
    t.amount      = amount;
    t.date        = Utils::getCurrentDateTime();
    t.balanceAfter = balAfter;
    transactionHistory.push_back(t);
    // Keep last 20 transactions
    if (transactionHistory.size() > 20) {
        transactionHistory.erase(transactionHistory.begin());
    }
}

// ─── Display ──────────────────────────────────────────────────────────────────

void Account::displayAccountInfo() const {
    std::cout << "\n";
    std::cout << "  ╔══════════════════════════════════════════╗\n";
    std::cout << "  ║           ACCOUNT INFORMATION            ║\n";
    std::cout << "  ╠══════════════════════════════════════════╣\n";
    std::cout << "  ║  Account No  : " << std::left << std::setw(26) << accountNumber << "║\n";
    std::cout << "  ║  Holder Name : " << std::setw(26) << holderName            << "║\n";
    std::cout << "  ║  Account Type: " << std::setw(26) << accountType           << "║\n";
    std::cout << "  ║  Balance     : " << std::setw(26) << Utils::formatCurrency(balance) << "║\n";
    std::cout << "  ║  Created     : " << std::setw(26) << createdDate           << "║\n";
    std::cout << "  ║  Status      : " << std::setw(26) << (isActive ? "ACTIVE" : "INACTIVE") << "║\n";
    std::cout << "  ╚══════════════════════════════════════════╝\n";
}

void Account::displayTransactionHistory() const {
    std::cout << "\n  ╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "  ║                  TRANSACTION HISTORY                    ║\n";
    std::cout << "  ╠══════════════════════════════════════════════════════════╣\n";
    if (transactionHistory.empty()) {
        std::cout << "  ║            No transactions found.                        ║\n";
    } else {
        std::cout << "  ║  " << std::left << std::setw(18) << "Date"
                  << std::setw(14) << "Type"
                  << std::setw(12) << "Amount"
                  << std::setw(14) << "Balance" << "║\n";
        std::cout << "  ╠══════════════════════════════════════════════════════════╣\n";
        for (const auto& t : transactionHistory) {
            std::string amt = Utils::formatCurrency(t.amount);
            std::string bal = Utils::formatCurrency(t.balanceAfter);
            std::cout << "  ║  " << std::setw(18) << t.date.substr(0, 16)
                      << std::setw(14) << t.type
                      << std::setw(12) << amt
                      << std::setw(14) << bal << "║\n";
        }
    }
    std::cout << "  ╚══════════════════════════════════════════════════════════╝\n";
}

// ─── Serialization ────────────────────────────────────────────────────────────

std::string Account::serialize() const {
    std::ostringstream oss;
    // Format: accNum|name|hashedPin|balance|type|date|isActive|txCount|tx1..
    oss << accountNumber << "|"
        << holderName    << "|"
        << pin           << "|"
        << std::fixed << std::setprecision(2) << balance << "|"
        << accountType   << "|"
        << createdDate   << "|"
        << (isActive ? 1 : 0) << "|"
        << transactionHistory.size();
    for (const auto& t : transactionHistory) {
        oss << "|" << t.type << "~" << t.amount << "~" << t.date << "~" << t.balanceAfter;
    }
    return oss.str();
}

Account Account::deserialize(const std::string& data) {
    Account acc;
    std::istringstream iss(data);
    std::string token;
    std::vector<std::string> parts;
    while (std::getline(iss, token, '|')) {
        parts.push_back(token);
    }
    if (parts.size() < 8) return acc;

    acc.accountNumber = std::stoi(parts[0]);
    acc.holderName    = parts[1];
    acc.pin           = parts[2];  // already hashed
    acc.balance       = std::stod(parts[3]);
    acc.accountType   = parts[4];
    acc.createdDate   = parts[5];
    acc.isActive      = (parts[6] == "1");
    int txCount       = std::stoi(parts[7]);

    for (int i = 0; i < txCount && (8 + i) < (int)parts.size(); i++) {
        std::istringstream txStream(parts[8 + i]);
        std::string txToken;
        std::vector<std::string> txParts;
        while (std::getline(txStream, txToken, '~')) {
            txParts.push_back(txToken);
        }
        if (txParts.size() == 4) {
            Transaction t;
            t.type         = txParts[0];
            t.amount       = std::stod(txParts[1]);
            t.date         = txParts[2];
            t.balanceAfter = std::stod(txParts[3]);
            acc.transactionHistory.push_back(t);
        }
    }
    return acc;
}
