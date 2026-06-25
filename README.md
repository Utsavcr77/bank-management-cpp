# 🏦 Bank Management System (C++)

A fully functional, console-based Bank Management System built with **C++17** using core OOP principles and persistent file-based storage.

---

## ✨ Features

| Feature | Description |
|---|---|
| **Create Account** | Open Savings or Current accounts with a secure 4-digit PIN |
| **Deposit** | Add funds to any account with PIN verification |
| **Withdraw** | Withdraw funds with balance validation |
| **Transfer** | Move funds between accounts |
| **Balance Check** | Real-time balance inquiry |
| **Transaction History** | Last 20 transactions with timestamps |
| **Change PIN** | Securely update account PIN |
| **Admin Panel** | View all accounts, bank summary, close accounts |
| **Persistent Storage** | All data saved to `data/bank_data.txt` |
| **PIN Security** | PINs stored using a hash function (never in plain text) |

---

## 🗂 Project Structure

```
bank-management/
├── include/
│   ├── Account.h       # Account class declaration
│   ├── Bank.h          # Bank class declaration
│   └── Utils.h         # Utility functions
├── src/
│   ├── main.cpp        # Entry point
│   ├── Account.cpp     # Account logic + serialization
│   ├── Bank.cpp        # Banking operations + menus
│   └── Utils.cpp       # Input helpers, formatting, hashing
├── data/               # Auto-created; stores bank_data.txt
├── Makefile
├── .gitignore
└── README.md
```

---

## 🔧 OOP Concepts Used

- **Encapsulation** — Private data members with public getters/setters
- **Abstraction** — `Bank` class hides low-level file and account logic
- **Modularity** — Separate classes for `Account`, `Bank`, and `Utils`
- **Serialization** — Custom `serialize()` / `deserialize()` for file I/O

---

## 🚀 Build & Run

### Prerequisites
- **macOS / Linux:** `g++` with C++17 support (comes with Xcode Command Line Tools on Mac)
- **Windows:** MinGW or MSVC

### Steps

```bash
# 1. Clone the repository
git clone https://github.com/YOUR_USERNAME/bank-management.git
cd bank-management

# 2. Build
make

# 3. Run
./bank_app
```

Or in one step: `make run`

### Windows (without make)
```cmd
g++ -std=c++17 -Wall -Iinclude src/main.cpp src/Account.cpp src/Bank.cpp src/Utils.cpp -o bank_app.exe
bank_app.exe
```

---

## 🔐 Admin Access

- **Admin Password:** `admin123`
- Change this in `src/Bank.cpp` → `adminMenu()` before deploying.

---

## 📋 Usage Guide

1. **Open New Account** → Enter name, choose Savings/Current, set 4-digit PIN, initial deposit
2. **Login** → Enter account number + PIN
3. Inside account menu: Deposit / Withdraw / Transfer / History / Change PIN
4. **Admin Panel** → View all accounts, bank summary, close accounts

---

## 📁 Data Storage Format

Each account is stored as one line in `data/bank_data.txt`:

```
accNum|holderName|hashedPin|balance|type|createdDate|isActive|txCount|tx1|tx2...
```

Transactions are stored as: `TYPE~amount~datetime~balanceAfter`

> ⚠️ `bank_data.txt` is excluded from Git via `.gitignore` — never commit real customer data to public repositories.

---

## 🛡 Security Notes

- PINs are hashed using a DJB2-based function before storage
- PIN input is hidden on terminal (characters shown as `*`)
- Data file excluded from version control
- *For production use, replace with bcrypt/SHA-256 and a real database*

---

## 📄 License

MIT License — free to use and modify for educational purposes.
