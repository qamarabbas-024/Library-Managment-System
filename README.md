<div align="center">

# 📚 Library Management System (LMS v2.0)
### *A Clean, Object-Oriented C++20 Console Project*

[![Language: C++20](https://img.shields.io/badge/Language-C%2B%2B20-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![CI / CD Build & Tests](https://img.shields.io/badge/CI%2FCD-10%2F10%20Passing-brightgreen?style=for-the-badge&logo=githubactions&logoColor=white)](.github/workflows/ci.yml)
[![Storage: CSV Flat-File](https://img.shields.io/badge/Storage-CSV%20Flat--File-purple?style=for-the-badge)](data/)
[![Platform: Cross-Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-blue?style=for-the-badge)](Makefile)

<br/>

**Developer: [Qamar Abbas](https://github.com/qamarabbas-024)**

</div>

---

## 📌 Project Overview

This is a comprehensive **Library Management System** developed in **Modern C++ (C++20)** using **Object-Oriented Programming (OOP)** principles. 

It manages the complete daily operations of a university or community library—including book cataloging, user authentication with password encryption, book loans, self-service renewals, hold queues, and overdue fine calculations—persisting all records directly into clean **CSV flat-files** with zero external database setups.

---

## ✨ Features

### 🎓 1. Student & Faculty Member Portal
* **Catalog Search**: Search books across Title, Author, Category, ISBN, or Book ID.
* **Borrowing Requests**: Apply to borrow books with automatic role policy enforcement:
  * **Students**: Up to 4 books for 14 days.
  * **Faculty**: Up to 8 books for 30 days.
* **Active Loans & Due Dates**: Track current loans with live calendar due dates.
* **Self-Service Renewals**: Extend loans directly from the terminal (up to 2 renewals per loan).
* **Hold Reservations**: Join a waitlist queue for books that are currently out of stock.
* **Account Management**: Update profile details and change password.
* **2-Factor Password Reset**: Secure password recovery verifying registered email and phone number.

### 🛡️ 2. Administrator Dashboard
* **Catalog Management**: Add new books, edit details, restock copy quantities, or remove books.
* **Circulation Control**: Review pending loan requests, approve/reject loans, and process book returns.
* **Overdue Fines**: Automatically calculate overdue fees ($0.50/day) on late returns and record fine payments.
* **Member Moderation**: View all members, add new users, and update account status (**Active**, **Suspended**, **Banned**).
* **Library Analytics**: View live statistics (total books, checked out copies, overdue loans, revenue).
* **Audit Logging**: Maintain a timestamped transaction log in `logs/audit.log`.

---

## 🏛️ Project Architecture & OOP Structure

The codebase is organized cleanly using modular Object-Oriented design:

```
Library-Managment-System/
├── include/
│   ├── models/                  # Domain Entities
│   │   ├── Book.h               # Book properties, copies & stock logic
│   │   ├── User.h               # User roles (Admin/Member) & borrowing limits
│   │   ├── Loan.h               # Loan dates, status & fine calculation
│   │   └── Reservation.h        # Waitlist reservation entity
│   ├── storage/                 # Data Access & Persistence
│   │   ├── IRepository.h        # Generic Repository interface
│   │   ├── CsvEngine.h          # Safe CSV file reader and writer
│   │   ├── BookRepository.h     # Book CSV storage and search
│   │   ├── UserRepository.h     # User CSV storage and lookups
│   │   ├── LoanRepository.h     # Loan CSV storage
│   │   ├── ReservationRepository.h # Reservation CSV storage
│   │   └── AuditLogger.h        # Append-only transaction logger
│   ├── services/                # Business Logic Services
│   │   ├── AuthService.h        # Login, signup, password hashing & 2FA reset
│   │   ├── BookService.h        # Catalog management & stock inventory
│   │   ├── MemberService.h      # Member profile & status moderation
│   │   ├── CirculationService.h # Loan approvals, returns, renewals, fines
│   │   └── ReportService.h      # Dashboard metrics & reporting
│   ├── utils/                   # Utilities
│   │   ├── DateTime.h           # Date arithmetic & calendar operations
│   │   ├── Crypto.h             # Pure C++ SHA-256 password hashing
│   │   ├── StringUtils.h        # String helpers & CSV parsing
│   │   ├── Terminal.h           # Terminal formatting & input validation
│   │   └── TableFormatter.h     # ASCII table layout generator
│   └── ui/
│       └── LibraryApp.h         # Interactive Terminal Menu & UI flows
├── src/                         # Implementation source files (.cpp)
├── data/                        # CSV storage files
│   ├── books.csv                # Book catalog database (40+ pre-seeded titles)
│   ├── users.csv                # User credentials & profiles
│   ├── loans.csv                # Active and past loan records
│   └── reservations.csv         # Book reservation waitlists
├── tests/
│   └── test_runner.cpp          # Automated unit test suite (10 tests)
├── build.bat                    # Windows build script
├── Makefile                     # Cross-platform Makefile for Linux/macOS
├── main.cpp                     # Application entry point
└── README.md                    # Project documentation
```

---

## 🚀 How to Build and Run

### Prerequisites
* Any standard **C++20** compiler (`g++`, `clang++`, or `MSVC`).
* No external libraries or database installations required.

### 🪟 Windows (Command Prompt / PowerShell)
```cmd
# Compile with build script
.\build.bat

# Run the program
.\build\LibrarySystem.exe
```

### 🐧 Linux / macOS
```bash
# Compile and run
make all
./build/LibrarySystem
```

---

## 🔑 Demo Login Accounts

| Role | Username | Password | Purpose |
| :--- | :--- | :--- | :--- |
| 🛡️ **Admin** | `qamarabbas` | `password` | Access Admin Dashboard & circulation controls |
| 🎓 **Student** | `student1` | `student123` | Student portal (4 books / 14 days limit) |
| 👨‍🏫 **Faculty** | `prof_smith` | `faculty123` | Faculty portal (8 books / 30 days limit) |
| ➕ **Register** | *Any* | *Any* | Choose **`[2] Register New Member`** on the main menu |

---

## 🧪 Automated Unit Tests

Run the included test runner to verify all core logic:

```powershell
# Run the 10 automated unit tests
.\build\test_runner.exe
```

```
==================================================
  RUNNING LMS AUTOMATED UNIT TESTS (C++20)
  Author: Qamar Abbas
==================================================

  [TEST] Date::today and string conversion ... [PASSED]
  [TEST] Date arithmetic (addDays & daysUntil) ... [PASSED]
  [TEST] Date comparison operators ... [PASSED]
  [TEST] SHA-256 deterministic hash ... [PASSED]
  [TEST] Salted password hashing & verification ... [PASSED]
  [TEST] StringUtils trim & case conversion ... [PASSED]
  [TEST] StringUtils substring search (case-insensitive) ... [PASSED]
  [TEST] CSV line parser with quoted commas ... [PASSED]
  [TEST] Book stock & borrowing invariants ... [PASSED]
  [TEST] User role borrowing limits & policies ... [PASSED]
  [TEST] Loan due date and overdue fine calculation ... [PASSED]
  [TEST] Full circulation flow (request -> approve -> return) ... [PASSED]
  [TEST] Loan renewal limits (max 2 renewals) ... [PASSED]
  [TEST] Book reservation waitlist queue on stock depletion ... [PASSED]
  [TEST] 2-Factor password reset verification (Email + Phone) ... [PASSED]

==================================================
  TEST SUMMARY:
  Total Tests : 10
  Passed      : 10
  Failed      : 0
==================================================
```

---

## 👨‍💻 Author

* **Developer**: [Qamar Abbas](https://github.com/qamarabbas-024)
* **Language**: Pure C++20
* **License**: MIT
