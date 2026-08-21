<div align="center">

# 📚 Library Management System (LMS)
### *A Production-Grade, Object-Oriented C++20 Library Automation Engine*

[![C++20](https://img.shields.io/badge/Language-C%2B%2B20-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![Build & Test CI](https://img.shields.io/badge/CI%2FCD-Passing-brightgreen?style=for-the-badge&logo=githubactions&logoColor=white)](.github/workflows/ci.yml)
[![Architecture](https://img.shields.io/badge/Architecture-Clean%20%2F%20Layered-orange?style=for-the-badge)](include/)
[![Storage](https://img.shields.io/badge/Storage-Flat--File%20CSV-purple?style=for-the-badge)](data/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-blue?style=for-the-badge)](Makefile)

<br/>

**Developed & Maintained by [Qamar Abbas](https://github.com/qamarabbas-024)**

</div>

---

## 🌟 Executive Summary

The **Library Management System (LMS v2.0)** is an industry-standard, cross-platform terminal software engineered entirely in modern **C++20**. Originally conceived as a foundational semester project, this enhanced edition has been re-architected from the ground up using **Clean Layered Architecture**, **Domain-Driven Design (DDD)**, **Cryptographic Security**, and real-world circulation mechanics.

It delivers zero-dependency flat-file data persistence with atomic transactions, role-based access control, automatic calendar-aware overdue fine calculation, book reservations with waitlists, and ANSI-colored interactive terminal dashboards.

---

## 🏛️ System Architecture

The project strictly follows the **Dependency Rule** and **Clean Architecture** patterns, separating concerns across four isolated layers:

```
┌─────────────────────────────────────────────────────────────┐
│                    Terminal Presentation (UI)               │
│               [ANSI Tables, Interactive Dashboards]         │
└──────────────────────────────┬──────────────────────────────┘
                               │ (calls)
                               ▼
┌─────────────────────────────────────────────────────────────┐
│                     Service Layer (Business Logic)          │
│   AuthService │ BookService │ MemberService │ Circulation  │
└──────────────────────────────┬──────────────────────────────┘
                               │ (operates on)
                               ▼
┌─────────────────────────────────────────────────────────────┐
│                        Domain Models                        │
│        Book │ User │ Member │ Admin │ Loan │ Reservation    │
└──────────────────────────────┬──────────────────────────────┘
                               │ (persisted by)
                               ▼
┌─────────────────────────────────────────────────────────────┐
│                  Storage Layer (Persistence)                │
│    IRepository<T> │ CsvEngine (Atomic I/O) │ AuditLogger    │
└─────────────────────────────────────────────────────────────┘
```

---

## ✨ Core Features & Real-World Mechanics

### 1. 📖 Catalog & Inventory Management
* **Rich Metadata**: ISBN-10/13, Accession IDs (`BK-001`), Title, Author, Category, Publication Year, Shelf Location (`Aisle A-1`).
* **Multi-Field Search**: Instant search across titles, authors, categories, ISBNs, and keywords.
* **Inventory Control**: Real-time stock tracking with total vs. available copy counters, auto-stock checks, and popularity ranking.

### 2. 🔐 Security & Role-Based Access Control (RBAC)
* **Cryptographic Security**: Salted **SHA-256** password hashing; passwords never stored in plain text.
* **Tiered Membership Policies**:
  * **Students**: Maximum 4 books for 14 days.
  * **Faculty**: Maximum 8 books for 30 days.
  * **General Members**: Maximum 2 books for 7 days.
  * **Admins / Librarians**: Complete administrative authority with audit logging.
* **Account Lifecycle**: Self-service registration, profile management, password recovery, and admin account moderation (Active, Suspended, Banned).

### 3. 🔄 Circulation Lifecycle State Machine
* **State Progression**: `REQUESTED` $\rightarrow$ `APPROVED` $\rightarrow$ `ISSUED` $\rightarrow$ `RETURNED` / `OVERDUE`.
* **Automated Overdue Fines**: Calendar-aware due date tracking with automatic daily fine accrual ($0.50/day) and fine payment processing.
* **Self-Service Renewals**: Allows loan extensions (up to 2 times) provided the item is not overdue or waitlisted by other members.
* **Waitlist Reservations**: Members can place reservations on borrowed-out books; the system notifies the waitlist automatically when copies are returned.

### 4. 📊 Analytics & Audit Logging
* **Executive Metrics**: Live stats for total book inventory, active checkouts, pending requests, overdue accounts, and collected fines.
* **Enterprise Audit Log**: Timestamped, append-only security and operational log (`logs/audit.log`) recording all logins, loan events, and administrative actions.

---

## 🗂️ Project Directory Structure

```
Library-Managment-System/
├── .github/
│   └── workflows/
│       └── ci.yml               # GitHub Actions CI automated build & test pipeline
├── data/
│   ├── books.csv                # Book catalog storage (40+ pre-seeded classics)
│   ├── users.csv                # User credentials & profile records
│   ├── loans.csv                # Active and past circulation loan records
│   └── reservations.csv         # Book waitlist reservations
├── include/
│   ├── models/                  # Domain entity definitions
│   │   ├── Book.hpp
│   │   ├── User.hpp
│   │   ├── Loan.hpp
│   │   └── Reservation.hpp
│   ├── storage/                 # Persistence layer & CSV engine
│   │   ├── IRepository.hpp
│   │   ├── CsvEngine.hpp
│   │   ├── BookRepository.hpp
│   │   ├── UserRepository.hpp
│   │   ├── LoanRepository.hpp
│   │   ├── ReservationRepository.hpp
│   │   └── AuditLogger.hpp
│   ├── services/                # Business logic services
│   │   ├── AuthService.hpp
│   │   ├── BookService.hpp
│   │   ├── MemberService.hpp
│   │   ├── CirculationService.hpp
│   │   └── ReportService.hpp
│   ├── utils/                   # C++20 utility toolset
│   │   ├── DateTime.hpp
│   │   ├── Crypto.hpp
│   │   ├── StringUtils.hpp
│   │   ├── Terminal.hpp
│   │   └── TableFormatter.hpp
│   └── ui/
│       └── LibraryApp.hpp       # Interactive Terminal UI controller
├── src/                         # Implementation source files (.cpp)
│   ├── models/
│   ├── storage/
│   ├── services/
│   ├── utils/
│   └── ui/
├── tests/
│   └── test_runner.cpp          # Automated unit test suite
├── .gitignore                   # Ignores build artifacts & temporary logs
├── build.bat                    # One-command Windows build script
├── Makefile                     # Cross-platform Makefile for Linux/macOS
├── main.cpp                     # Application entry point & dependency injection container
└── README.md                    # Project documentation
```

---

## 🚀 Quickstart & Building

### Prerequisites
* A modern C++ compiler supporting **C++20** (`g++ >= 11`, `clang++ >= 13`, or MSVC 2022).
* No external libraries or database servers required.

### 🪟 Windows Build (One-Command)
```cmd
.\build.bat
.\build\LibrarySystem.exe
```

### 🐧 Linux / macOS Build
```bash
make all
./build/LibrarySystem
```

### 🧪 Running Automated Unit Tests
```cmd
# Windows
g++ -std=c++20 -Wall -Wextra -O2 -Iinclude src\utils\*.cpp src\models\*.cpp src\storage\*.cpp src\services\*.cpp tests\test_runner.cpp -o build\test_runner.exe
.\build\test_runner.exe

# Linux / macOS
g++ -std=c++20 -Wall -Wextra -O2 -Iinclude src/utils/*.cpp src/models/*.cpp src/storage/*.cpp src/services/*.cpp tests/test_runner.cpp -o build/test_runner
./build/test_runner
```

---

## 🔑 Default Credentials

| Role | Username | Password | Notes |
| :--- | :--- | :--- | :--- |
| **Administrator** | `qamarabbas` | `password` | Full administrative control & reporting |
| **New Member** | *Any* | *Any* | Use the **[2] Register New Member** option on startup |

---

## 👨‍💻 Author & Project History

* **Author**: [Qamar Abbas](https://github.com/qamarabbas-024)
* **Project Heritage**: Developed as a 1st-semester university project, refactored and elevated to an industry-grade C++20 enterprise portfolio application.
* **License**: Open-source educational showcase.
