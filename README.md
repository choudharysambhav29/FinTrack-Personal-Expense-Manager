# 💰 FinTrack – Personal Expense Manager

<p align="center">
  <img src="https://img.shields.io/badge/Language-C11-blue.svg" alt="Language C11" />
  <img src="https://img.shields.io/badge/Compiler-GCC%2014.2%2B-green.svg" alt="Compiler GCC" />
  <img src="https://img.shields.io/badge/Standard-ISO%2FIEC%209899%3A2011-orange.svg" alt="C11 Standard" />
  <img src="https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg" alt="Platform Cross-Platform" />
  <img src="https://img.shields.io/badge/License-MIT-purple.svg" alt="License MIT" />
</p>

A modular, menu-driven **C11 console application** engineered for personal finance management, category-wise budget enforcement, statistical spending analytics, and dynamic **HTML dashboard reporting**.

Designed as a college-level academic project demonstrating foundational C programming principles: **structures**, **static and multidimensional arrays**, **modular functions**, **dual text (CSV) and binary (.dat) file handling**, **pointer mechanics**, and **safe input stream processing**.

---

## 📌 Table of Contents
1. [Core Features](#-core-features)
2. [C Programming Concepts Implemented](#-c-programming-concepts-implemented)
3. [Architecture & File Structure](#-architecture--file-structure)
4. [Data Storage & Formats](#-data-storage--formats)
5. [Installation & Build Instructions](#-installation--build-instructions)
6. [Interactive Terminal Walkthrough](#-interactive-terminal-walkthrough)
7. [HTML Dashboard Export (Chart.js)](#-html-dashboard-export-chartjs)
8. [Viva & Academic Defense Guide](#-viva--academic-defense-guide)
9. [License](#-license)

---

## 🚀 Core Features

### 1. Expense Tracking (CRUD Operations)
- **Add Expense**: Automatic sequential ID generation (`#1`, `#2`, ...), calendar-aware date validation (`YYYY-MM-DD` with leap year logic), numbered category picker (1–7), positive monetary amount validation, and memo.
- **View All Expenses**: Aligned tabular layout showing `ID | Date | Category | Amount (Rs.) | Note` with dynamic column spacing and grand totals.
- **Edit Expense**: Search by ID and selectively update any field (Date, Category, Amount, or Note).
- **Delete Expense**: Search by ID with confirmation prompt (`[1] Yes, [2] No`) and contiguous array element shifting.

### 2. Predefined Categories & Monthly Filtering
- Strict enumeration across 7 standard categories: `Food`, `Travel`, `Shopping`, `Bills`, `Entertainment`, `Health`, and `Other`.
- Summary aggregation computing transaction counts, total expenditures, and percentage shares.
- Filterable **all-time** or for any specific month (`YYYY-MM`).

### 3. Monthly Budget Manager & Warning Alerts
- Set and update monthly spending limits for any `YYYY-MM`.
- Real-time spend vs. budget calculation with deficit calculation.
- Automated threshold warning indicators:
  - `[OK - WITHIN BUDGET]`: Spending is below 80% of monthly cap.
  - `[WARNING - 80%+ USAGE]`: Caution alert when spending reaches 80% to 99.9%.
  - `[ALERT - 100%+ USAGE]`: Over-budget alert showing the exact deficit amount.

### 4. Interactive Text-Based ASCII Charts
- **Category Bar Chart**: Horizontal bar chart rendered with `#` characters, dynamically scaled to the highest spending category (maximum width: 40 characters), showing exact amounts and percentage shares.
- **Monthly Trend Chart**: Automatically discovers unique months and sorts them chronologically (`YYYY-MM`), visualizing spending trajectory over time.

### 5. Spending Pattern Analytics
- **Highest Spending Category**: Highlights peak category expenditure and share.
- **Daily Average Spend**: Clearly compares:
  - *Method A (Active Days)*: Total spend divided by unique calendar dates with at least one transaction.
  - *Method B (Calendar Days)*: Total spend divided by all calendar days in the month/period (e.g., 30 days for November).
- **Top 3 Largest Expenses**: Ranks the top 3 individual transactions.
- **Month-over-Month (MoM) % Change**: Measures spending increase or decrease compared to the preceding calendar month.

### 6. Interactive HTML5 Dashboard Export
- One-click export (`report.html`) generated directly from pure C file streams.
- Standalone responsive layout with modern CSS (cards, badges, progress bars).
- Integrated **Chart.js CDN**:
  - Interactive **Category Doughnut Chart**.
  - Interactive **Monthly Spending Bar Chart**.
- Recent transactions table (10 most recent records) and budget utilization table.

---

## 🧠 C Programming Concepts Implemented

| Concept | File Location | Academic Explanation |
|---|---|---|
| **Structures (`struct`)** | `expense.h`, `budget.h`, `analysis.h` | Bundles heterogeneous data types (int, char arrays, double) into cohesive entities (`Expense`, `Budget`, `CategorySummary`). |
| **Arrays of Structures** | `main.c`, `expense.c`, `budget.c` | Fixed-size contiguous stack arrays (`Expense expenses[MAX_EXPENSES]`) bounded by compile-time macros (`#define MAX_EXPENSES 1000`). |
| **Pointers & Pass-by-Reference** | All modules | Passes memory addresses (`int *count`, `double *out_val`) allowing functions to mutate caller state and prevent memory duplication. |
| **Text File I/O (`.csv`)** | `fileio.c` | Reads/writes CSV text files using `fopen()`, `fprintf()`, `fgets()`, and custom comma tokenization (`strchr`). |
| **Binary File I/O (`.dat`)** | `fileio.c` | Directly reads and writes memory blocks using `fwrite()` and `fread()` in binary modes (`"wb"`, `"rb"`). |
| **String Manipulation** | `expense.c`, `analysis.c` | Validates dates and strings using `strncpy`, `strlen`, `strncmp`, `strcmp`, `sscanf`, and `snprintf`. |
| **Input Buffer Sanitization** | `expense.c` | Custom `read_line()` strips trailing newlines and flushes residual characters from `stdin` to eliminate infinite loops. |
| **Algorithms** | `chart.c`, `analysis.c` | Implements Bubble Sort on ISO-8601 month strings and dynamic range normalization for ASCII bar scaling. |

---

## 📂 Architecture & File Structure

```
├── main.c              # Application entry point, main loop, and UI menu dispatcher
├── expense.h / .c      # Expense struct, CRUD operations, calendar validation, input sanitizers
├── budget.h / .c       # Budget struct, budget tracking, 80%/100% threshold warning system
├── analysis.h / .c     # Category aggregation, daily averages, top 3, MoM trend calculations
├── chart.h / .c        # Dynamic ASCII category bar charts and chronological trend charts
├── export.h / .c       # Pure C generator for standalone HTML dashboard (Chart.js CDN)
├── fileio.h / .c       # Persistence engine: expenses.csv (CSV parser) & budget.dat (binary I/O)
├── Makefile            # Build configuration with gcc -Wall -Wextra -std=c11
├── .gitignore          # Excludes object files (.o), binaries (.exe), and local reports
└── README.md           # Comprehensive project documentation
```

---

## 💾 Data Storage & Formats

### 1. `expenses.csv` (Human-Readable Text)
```csv
id,date,category,amount,note
1,2026-11-20,Food,250.00,Team lunch
2,2026-11-21,Travel,120.00,Metro pass
```

### 2. `budget.dat` (Compact Binary Serialization)
- **Header**: 4-byte integer storing budget count (`sizeof(int)`).
- **Body**: Contiguous array of raw `Budget` structs (`sizeof(Budget) * count`).

---

## 🛠️ Installation & Build Instructions

### Prerequisites
- GCC Compiler (MinGW-w64 on Windows, or default GCC on Linux/macOS).
- GNU Make (optional, direct GCC command supported).

### Compilation

#### Method 1: Using Make (Recommended)
```bash
# Build the application
make

# Clean object files and executables
make clean

# Build and run
make run
```
*(On Windows with MinGW, use `mingw32-make` instead of `make`)*

#### Method 2: Direct GCC Compilation
```bash
gcc -Wall -Wextra -std=c11 -o fintrack main.c expense.c budget.c analysis.c chart.c export.c fileio.c
```

#### Running the Executable
```bash
# Windows
.\fintrack.exe

# Linux / macOS
./fintrack
```

---

## 🖥️ Interactive Terminal Walkthrough

```text
=======================================================
         FINTRACK - PERSONAL EXPENSE MANAGER           
=======================================================
  [1]  Add New Expense
  [2]  View All Expenses
  [3]  Edit Expense by ID
  [4]  Delete Expense by ID
  [5]  Category Spending Summary (Totals & % Share)
  [6]  Monthly Budget Manager
  [7]  Category Spending Bar Chart (ASCII #)
  [8]  Monthly Spending Trend Chart (ASCII #)
  [9]  Spending Pattern Analysis & Insights
  [10] Export HTML Dashboard ('report.html')
  [11] Save All Data to Files
  [0]  Save & Exit
=======================================================
```

---

## 📊 HTML Dashboard Export (Chart.js)

Selecting Option `[10]` generates `report.html`. Opening this file in any modern web browser presents:

- **Executive KPI Cards**: Total Spent (`Rs.`), Total Transactions, Highest Spending Category, Average Spend per Expense.
- **Category Doughnut Chart**: Visually balanced distribution across all 7 categories.
- **Monthly Spending Bar Chart**: Trajectory of expenditure across consecutive months.
- **Budget Tracking Table**: Real-time progress bars and status tags (`WITHIN BUDGET`, `WARNING >= 80%`, `OVER BUDGET`).
- **Recent Transactions**: Up to 10 most recent expense entries.

---

## 🎓 Viva & Academic Defense Guide

### Top 5 Viva Questions & Short Answers

1. **Why separate the codebase into multiple `.c` and `.h` files?**
   *Answer*: Promotes modular programming, separation of concerns, and faster compilation. Each module handles a single responsibility (e.g., `fileio.c` handles disk persistence, `chart.c` handles visualization).

2. **How does the application prevent crashes when users enter letters instead of numbers?**
   *Answer*: Standard `scanf("%d")` can enter an infinite loop on invalid input. FinTrack reads input lines as strings using `fgets()` via `read_line()`, and parses numbers using `strtol()` and `strtod()`, rejecting non-numeric characters gracefully.

3. **Why use both CSV text and binary `.dat` file formats?**
   *Answer*: To demonstrate both text-stream parsing (`fgets`, `strchr`) for portable human-readable data (`expenses.csv`), and binary block I/O (`fwrite`, `fread`) for high-speed, compact struct persistence (`budget.dat`).

4. **How are deleted expenses handled in memory?**
   *Answer*: When an expense is deleted, array elements after the target index are shifted one position to the left (`expenses[i] = expenses[i + 1]`), and the active count is decremented.

5. **How does the pure C backend render browser charts?**
   *Answer*: C writes the HTML file stream directly using `fprintf()`, embedding CSS styling and injecting C data arrays into JavaScript literals read by the client-side Chart.js library.

---

## 📄 License
This project is open-source and available under the [MIT License](LICENSE).
