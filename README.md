<div align="center">

# 🏛️ Enterprise Library Management System (LMS v2.0)
### *A Production-Grade, Object-Oriented C++20 Integrated Library System (ILS)*

[![Language: C++20](https://img.shields.io/badge/Language-C%2B%2B20-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![CI / CD Build & Tests](https://img.shields.io/badge/CI%2FCD-17%2F17%20Passing-brightgreen?style=for-the-badge&logo=githubactions&logoColor=white)](.github/workflows/ci.yml)
[![Architecture: Clean Layered](https://img.shields.io/badge/Architecture-Clean%20Layered%20%2F%20DDD-orange?style=for-the-badge)](include/)
[![Storage: Zero-DB Flat-File CSV](https://img.shields.io/badge/Storage-Atomic%20CSV-purple?style=for-the-badge)](data/)
[![Platform: Cross-Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-blue?style=for-the-badge)](Makefile)

<br/>

**Architect & Principal Developer: [Qamar Abbas](https://github.com/qamarabbas-024)**

</div>

---

## 📖 Table of Contents
1. [Real-World ILS Overview](#-real-world-ils-overview)
2. [Real-World vs. C++ Architecture Mapping](#-real-world-vs-c-architecture-mapping)
3. [System Architecture](#-system-architecture)
4. [File Hierarchy & Directory Blueprint](#-file-hierarchy--directory-blueprint)
5. [Domain Mechanics & Business Logic](#-domain-mechanics--business-logic)
   * [Borrowing Privilege Policy Matrix](#1-borrowing-privilege-policy-matrix)
   * [Circulation Life-Cycle State Machine](#2-circulation-life-cycle-state-machine)
   * [Automated Overdue Fine & Accounting Engine](#3-automated-overdue-fine--accounting-engine)
   * [Waitlist & Hold Reservation Queue](#4-waitlist--hold-reservation-queue)
   * [OPAC Discovery & Levenshtein Fuzzy Search](#5-opac-discovery--levenshtein-fuzzy-search)
   * [Affinity-Based AI Recommendation Engine](#6-affinity-based-ai-recommendation-engine)
   * [Cryptographic Security & 2FA Recovery](#7-cryptographic-security--2fa-recovery)
6. [Quickstart & Build Instructions](#-quickstart--build-instructions)
7. [Default Credentials & Test Personas](#-default-credentials--test-personas)
8. [Automated Test Suite (17 Tests)](#-automated-test-suite-17-tests)
9. [Author & Heritage](#-author--heritage)

---

## 🌐 Real-World ILS Overview

In real-world library operations (modeled after systems like **Koha**, **Evergreen**, and **Ex Libris Alma**), an **Integrated Library System (ILS)** consists of modular subsystems:
- **Cataloging & Accessioning**: Tracking unique accession numbers, ISBNs, DDC/Shelf locations, and copy counts.
- **Patron Management**: Administering membership tiers, identity verification, active loan caps, and privilege states.
- **Circulation Desk**: Governing loans, returns, self-service renewals, hold queues, and daily overdue penalties.
- **OPAC (Public Catalog)**: Enabling member discovery with typo-tolerant search and personalized reading suggestions.
- **Auditing & Accounting**: Generating official receipts, slips, and recording an append-only audit trail of transactions.

This project delivers a **100% Pure Standard C++20** implementation of this complete ecosystem with **zero external database dependencies**, using atomic flat-file CSV storage.

---

## 🗺️ Real-World vs. C++ Architecture Mapping

| Real-World ILS Concept | C++ Domain Entity / Service | Implementation Details |
| :--- | :--- | :--- |
| **Accession Register & Catalog** | [`Book.h`](include/models/Book.h) / [`BookService.h`](include/services/BookService.h) | Accession ID (`BK-001`), ISBN, Title, Author, Category, Shelf Location, Stock Invariants. |
| **Patron / Member Administration** | [`User.h`](include/models/User.h) / [`MemberService.h`](include/services/MemberService.h) | Tiered roles (Student, Faculty, General), status flags (Active, Suspended, Banned), loan counters. |
| **Circulation & Hold Shelf** | [`Loan.h`](include/models/Loan.h) / [`CirculationService.h`](include/services/CirculationService.h) | Loan state machine (`Requested` $\rightarrow$ `Approved` $\rightarrow$ `Issued` $\rightarrow$ `Returned` / `Overdue`). |
| **Hold Queue / Reservations** | [`Reservation.h`](include/models/Reservation.h) | FIFO waitlist queue for out-of-stock items; automatically notifies waitlisted patrons upon return. |
| **Circulation Policies & Fines** | [`Loan.h`](include/models/Loan.h) / [`CirculationService.h`](include/services/CirculationService.h) | Calendar date math (`std::chrono`), auto $0.50/day fine accrual, payment tracking. |
| **OPAC Discovery & Search** | [`StringUtils.h`](include/utils/StringUtils.h) / [`BookRepository.h`](include/storage/BookRepository.h) | Dynamic programming **Levenshtein Distance** for typo-tolerant fuzzy matching. |
| **Collection Recommendations** | [`RecommendationService.h`](include/services/RecommendationService.h) | Collaborative filtering & category/author affinity scoring from borrowing history. |
| **Digital Library Cards & Slips** | [`Barcode.h`](include/utils/Barcode.h) / [`ReceiptService.h`](include/services/ReceiptService.h) | Formats and exports ASCII checkout receipts, return slips, and digital ID cards. |
| **Security & Compliance** | [`Crypto.h`](include/utils/Crypto.h) / [`AuditLogger.h`](include/storage/AuditLogger.h) | Salted **SHA-256** password hashing and timestamped append-only audit trail (`logs/audit.log`). |

---

## 🏛️ System Architecture

The software is engineered using **Clean Layered Architecture** and **Domain-Driven Design (DDD)**:

```
┌─────────────────────────────────────────────────────────────────────────┐
│                       PRESENTATION LAYER (TUI)                          │
│        [Terminal.h │ TableFormatter.h │ Barcode.h │ LibraryApp.h]       │
└────────────────────────────────────┬────────────────────────────────────┘
                                     │ (invokes)
                                     ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                      APPLICATION & SERVICE LAYER                        │
│   AuthService │ BookService │ MemberService │ CirculationService        │
│   ReportService │ RecommendationService │ ReceiptService                │
└────────────────────────────────────┬────────────────────────────────────┘
                                     │ (orchestrates)
                                     ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                          DOMAIN MODEL LAYER                             │
│          Book.h │ User.h │ Loan.h │ Reservation.h │ DateTime.h          │
└────────────────────────────────────┬────────────────────────────────────┘
                                     │ (persisted by)
                                     ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                     STORAGE & PERSISTENCE LAYER                         │
│       IRepository<T> │ CsvEngine (Atomic I/O) │ AuditLogger.h           │
│       data/books.csv │ data/users.csv │ data/loans.csv │ data/res.csv   │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 🗂️ File Hierarchy & Directory Blueprint

```
Library-Managment-System/
├── .github/
│   └── workflows/
│       └── ci.yml               # Automated GitHub Actions cross-platform build & test CI
├── .vscode/
│   └── c_cpp_properties.json   # Pre-configured C++20 IntelliSense path definitions
├── data/
│   ├── books.csv                # Catalog database with 40+ curated volume records
│   ├── users.csv                # Member profiles, roles, and salted password hashes
│   ├── loans.csv                # Active and historical circulation loan records
│   └── reservations.csv         # Book hold and waitlist queue records
├── exports/                     # Generated ASCII loan receipts and return slips (.txt)
├── logs/
│   └── audit.log                # Append-only enterprise transaction and security log
├── include/
│   ├── models/                  # Strong-typed domain entities & invariants
│   │   ├── Book.h               # Book entity (Stock, ISBN, Location, Borrow Count)
│   │   ├── User.h               # User entity (Roles, Policies, Account Status)
│   │   ├── Loan.h               # Loan transaction entity & state transitions
│   │   └── Reservation.h        # Book waitlist and hold reservation entity
│   ├── storage/                 # Data persistence and flat-file storage engine
│   │   ├── IRepository.h        # Generic repository interface template
│   │   ├── CsvEngine.h          # Atomic file writer (writes to .tmp then renames)
│   │   ├── BookRepository.h     # Book storage with indexed category & fuzzy search
│   │   ├── UserRepository.h     # User storage with username & email indexing
│   │   ├── LoanRepository.h     # Loan storage with active user loan lookups
│   │   ├── ReservationRepository.h # Hold queue persistence & priority ordering
│   │   └── AuditLogger.h        # Append-only transaction audit logging engine
│   ├── services/                # Business logic and application services
│   │   ├── AuthService.h        # Authentication, 2FA password reset, sessions
│   │   ├── BookService.h        # Catalog management, inventory control, stats
│   │   ├── MemberService.h      # Member lifecycle, moderation (ban/unban/suspend)
│   │   ├── CirculationService.h # Loan approvals, returns, renewals, hold alerts
│   │   ├── ReportService.h      # Library analytics and executive metrics
│   │   ├── RecommendationService.h # AI Reading recommendation engine
│   │   └── ReceiptService.h     # ASCII receipt formatter & file exporter
│   ├── utils/                   # Reusable C++20 toolset
│   │   ├── DateTime.h           # Date arithmetic & ISO calendar calculations
│   │   ├── Crypto.h             # Pure C++ SHA-256 & salt generator
│   │   ├── StringUtils.h        # CSV escaping, trimming, Levenshtein distance
│   │   ├── Terminal.h           # Crash-proof stream input & ANSI colors
│   │   ├── TableFormatter.h     # Dynamic auto-scaling ASCII table renderer
│   │   └── Barcode.h            # ASCII Code-39 barcode & card generator
│   └── ui/
│       └── LibraryApp.h         # Terminal UI controller & menu screens
├── src/                         # Implementation source files (.cpp)
│   ├── models/
│   ├── storage/
│   ├── services/
│   ├── utils/
│   └── ui/
├── tests/
│   └── test_runner.cpp          # 17 automated unit and integration tests
├── .gitignore                   # Ignores build binaries and runtime logs
├── build.bat                    # One-command Windows build script
├── Makefile                     # Cross-platform Makefile for Linux/macOS
├── main.cpp                     # Application entry point & dependency injection container
└── README.md                    # System documentation
```

---

## ⚙️ Domain Mechanics & Business Logic

### 1. Borrowing Privilege Policy Matrix

The system enforces strict borrowing privileges based on the member's classification:

| Patron Category | Max Active Loans | Loan Duration | Renewal Cap | Daily Overdue Fine |
| :--- | :---: | :---: | :---: | :---: |
| 🎓 **Student Member** | **4 Books** | **14 Days** | Up to 2 times | $0.50 / day |
| 👨‍🏫 **Faculty Member** | **8 Books** | **30 Days** | Up to 2 times | $0.50 / day |
| 👤 **General Patron** | **2 Books** | **7 Days** | Up to 2 times | $0.50 / day |
| 🛡️ **Administrator** | Unlimited | Administrative | N/A | N/A |

---

### 2. Circulation Life-Cycle State Machine

Loans progress through deterministic, auditable states:

```
[PATRON] Request Loan  ──> (Status: REQUESTED)
                                  │
                                  ▼
[ADMIN]  Approve / Reject ──> (Status: APPROVED / REJECTED)
                                  │
                                  ▼
[PATRON] Collect Book  ──> (Status: ISSUED) ──[Renew]──> (Extends Due Date)
                                  │
                  ┌───────────────┴───────────────┐
                  ▼                               ▼
      (Returned On Time)                  (Returned Late)
       Status: RETURNED                   Status: OVERDUE
       [Restocks Copy]                    [Accrues $0.50/day Fine]
```

---

### 3. Automated Overdue Fine & Accounting Engine
* Due dates are computed using `std::chrono` calendar math.
* If a loan exceeds its due date, the system accrues **$0.50 per day**.
* When books are returned, total unpaid fines remain attached to the member record until settled via the **`Process Fine Payment`** desk.

---

### 4. Waitlist & Hold Reservation Queue
* If all physical copies of a book are checked out (`availableCopies == 0`), members can place a **Hold Reservation**.
* The moment any copy is returned, the system automatically checks the waitlist queue, assigns priority to the earliest reservation, and notifies the member upon login.

---

### 5. OPAC Discovery & Levenshtein Fuzzy Search
The search engine utilizes dynamic programming **Levenshtein Distance**:
$$\text{Similarity}(s_1, s_2) = 1.0 - \frac{\text{LevenshteinDistance}(s_1, s_2)}{\max(|s_1|, |s_2|)}$$
* Searching for typos like **`"Atmoic Habbits"`** or **`"Clean Cod"`** instantly finds the correct books with $\ge 60\%$ similarity.

---

### 6. Affinity-Based AI Recommendation Engine
The `RecommendationService` analyzes past loans and computes affinity scoring:
$$\text{Score}(Book) = (\text{CategoryAffinity} \times 2) + (\text{AuthorAffinity} \times 3) + \text{PopularityBoost}$$
Members receive personalized reading suggestions based on their literary preferences.

---

### 7. Cryptographic Security & 2FA Recovery
* **Password Hashing**: Passwords are never stored in plaintext. They are salted with a 16-byte cryptographic salt and hashed using **SHA-256**.
* **2-Factor Password Reset**: Password recovery requires **both registered Email and registered Phone Number** matching before resetting credentials.

---

## 🚀 Quickstart & Build Instructions

### Prerequisites
* A standard **C++20** compiler (`g++ >= 11`, `clang++ >= 13`, or MSVC 2022).
* No external database installations or third-party packages required.

### 🪟 Windows (Command Prompt / PowerShell)
```cmd
.\build.bat
.\build\LibrarySystem.exe
```

### 🐧 Linux / macOS / WSL
```bash
make all
./build/LibrarySystem
```

---

## 🔑 Default Credentials & Test Personas

| Role / Persona | Username | Password | Access Rights |
| :--- | :--- | :--- | :--- |
| 🛡️ **Administrator** | `qamarabbas` | `password` | Full Catalog Control, Loan Approvals, Audit Logs, Statistics |
| 🎓 **Student Member** | `student1` | `student123` | Student Portal, AI Recommendations, Digital ID Card (4 Books / 14 Days) |
| 👨‍🏫 **Faculty Member** | `prof_smith` | `faculty123` | Faculty Portal, Extended Loan Privileges (8 Books / 30 Days) |
| ➕ **New Account** | *Self-Register* | *Your Choice* | Select **`[2] Register New Member`** on the startup menu |

---

## 🧪 Automated Test Suite (17 Tests)

The system includes a self-contained unit and integration testing engine:

```powershell
# Run test suite
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
  [TEST] Levenshtein distance & fuzzy search ... [PASSED]
  [TEST] ASCII Barcode & Card rendering ... [PASSED]
  [TEST] CSV line parser with quoted commas ... [PASSED]
  [TEST] Book stock & borrowing invariants ... [PASSED]
  [TEST] User role borrowing limits & policies ... [PASSED]
  [TEST] Loan due date and overdue fine calculation ... [PASSED]
  [TEST] Full circulation flow (request -> approve -> return) ... [PASSED]
  [TEST] Loan renewal limits and policy enforcement ... [PASSED]
  [TEST] Book reservation waitlist queue on stock depletion ... [PASSED]
  [TEST] RecommendationService affinity scoring ... [PASSED]
  [TEST] ReceiptService format and file export ... [PASSED]

==================================================
  TEST SUMMARY:
  Total Tests : 17
  Passed      : 17
  Failed      : 0
==================================================
```

---

## 👨‍💻 Author & Heritage

* **Architect & Principal Developer**: [Qamar Abbas](https://github.com/qamarabbas-024)
* **Project Heritage**: Developed as a 1st-semester foundational university project, refactored and elevated to an industry-standard, production-grade C++20 software engineering masterpiece.
* **License**: Open-source portfolio showcase.
